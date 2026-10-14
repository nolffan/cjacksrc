// ----------------------------------------------------------------------- //
//
// MODULE  : GameSpyServer.cpp
//
// PURPOSE : IGameSpyServer on GameSpy's query and reporting 2, NAT
//           negotiation and CD key SDKs.
//
// cjacksrc: Monolith's source is lost; reconstructed from the retail
// GameSpy.dll (1.1), 0x10003890..0x1000606b. The server reports the
// properties the game sets, answers the master server and clients from a
// thread of its own, and checks the clients' CD keys with GameSpy's key
// server.
//
// ----------------------------------------------------------------------- //

#include "GameSpyServer.h"
#include "GameSpySDK/gcdkey/gcdkeys.h"
#include "GameSpySDK/qr2/qr2regkeys.h"

// The server thread's beat.
#define THREAD_THINK_PERIOD		10

// How long a dying thread is waited for.
#define THREAD_END_TIMEOUT		1000

// A CD key challenge: this many random lower case letters.
#define CDKEY_CHALLENGE_LEN		8

// RETAIL: 0x100038e0
void IGameSpyServer::Delete( IGameSpyServer* pGameSpyServer )
{
	if( pGameSpyServer )
		delete pGameSpyServer;
}

// RETAIL: 0x100038f0
bool CGameSpyServer::Unpublish( )
{
	if( m_ePublishStatus != ePublishStatus_Idle )
	{
		if( m_hThread )
		{
			m_ThreadEndEvent.Set( );
			if( WaitForSingleObject( m_hThread, THREAD_END_TIMEOUT ) == WAIT_TIMEOUT )
				TerminateThread( m_hThread, 0 );
			CloseHandle( m_hThread );
			m_hThread = NULL;
		}

		if( m_bCDKeyChecks )
		{
			gcd_shutdown( );
			m_bCDKeyChecks = false;
		}

		qr2_shutdown( NULL );
		NNFreeNegotiateList( );

		m_ePublishStatus = ePublishStatus_Idle;
	}

	return true;
}

// RETAIL: 0x10003970
DWORD CGameSpyServer::ThreadMain( )
{
	m_ThreadStartedEvent.Set( );

	do
	{
		CWinSync_CSAuto lockData( m_csData );

		qr2_think( NULL );
		if( m_bCDKeyChecks )
			gcd_think( );
		NNThink( );
	}
	while( WaitForSingleObject( m_ThreadEndEvent.GetEvent( ), THREAD_THINK_PERIOD ) == WAIT_TIMEOUT );

	return 0;
}

// RETAIL: 0x100039d0
void CGameSpyServer::SetNumPlayers( int nNumPlayers )
{
	CWinSync_CSAuto lockData( m_csData );

	m_nNumPlayers = nNumPlayers;

	char szValue[256];
	sprintf( szValue, "%d", nNumPlayers );
	SetServerProperty( "numplayers", szValue );
}

// RETAIL: 0x10003a30
void CGameSpyServer::SetNumTeams( int nNumTeams )
{
	CWinSync_CSAuto lockData( m_csData );

	m_nNumTeams = nNumTeams;

	char szValue[256];
	sprintf( szValue, "%d", nNumTeams );
	SetServerProperty( "numteams", szValue );
}

// RETAIL: 0x10003a90
bool CGameSpyServer::GetCDKeyChallenge( char* pszChallenge, unsigned int* pnChallengeLen )
{
	if( !m_bCDKeyChecks )
		return false;

	unsigned int nChallengeLen = *pnChallengeLen;
	*pnChallengeLen = CDKEY_CHALLENGE_LEN;

	// Just asking for the length?
	if( !pszChallenge )
		return true;

	if( nChallengeLen < CDKEY_CHALLENGE_LEN )
		return false;

	// RETAIL: the terminator makes the challenge one byte longer than the
	// length it reports.
	unsigned int nChar;
	for( nChar = 0; nChar < CDKEY_CHALLENGE_LEN; nChar++ )
	{
		pszChallenge[nChar] = 'a' + ( rand( ) % 26 );
	}
	pszChallenge[nChar] = '\0';

	return true;
}

// RETAIL: 0x10003af0
bool CGameSpyServer::HandleNetMessage( unsigned char const* pData, unsigned int nDataLen,
	unsigned char const* pSenderAddr, unsigned short nSenderPort )
{
	// Queries: qr2's (0xfe 0xfd), or the old protocol's (';' or '\').
	bool bQuery = ( nDataLen >= 1 && pData[0] == ';' ) ||
		( nDataLen >= 2 && *( unsigned short const* )pData == 0xfdfe ) ||
		( nDataLen >= 1 && pData[0] == '\\' );

	// NAT negotiation packets.
	bool bNatNegotiation = ( nDataLen >= 2 && pData[0] == NN_MAGIC_0 && pData[1] == NN_MAGIC_1 );

	if( !bQuery && !bNatNegotiation )
		return false;

	sockaddr_in senderAddr;
	memset( &senderAddr, 0, sizeof( senderAddr ));
	senderAddr.sin_family = AF_INET;
	senderAddr.sin_addr.s_addr = htonl(( pSenderAddr[0] << 24 ) | ( pSenderAddr[1] << 16 ) | ( pSenderAddr[2] << 8 ) | pSenderAddr[3] );
	senderAddr.sin_port = htons( nSenderPort );

	if( bQuery )
		qr2_parse_query( NULL, ( char* )pData, nDataLen, ( sockaddr* )&senderAddr );
	else
		NNProcessData(( char* )pData, nDataLen, &senderAddr );

	return true;
}

// RETAIL: 0x10003be0
int CGameSpyServer::CountCallback( qr2_key_type keyType, void* pUserData )
{
	CGameSpyServer* pGameSpyServer = ( CGameSpyServer* )pUserData;

	CWinSync_CSAuto lockData( pGameSpyServer->m_csData );

	switch( keyType )
	{
		case key_player:
			return pGameSpyServer->m_nNumPlayers;
		case key_team:
			return pGameSpyServer->m_nNumTeams;
	}

	return 0;
}

// RETAIL: 0x10003c30
void CGameSpyServer::AddErrorCallback( qr2_error_t error, char* pszErrMsg, void* pUserData )
{
	CGameSpyServer* pGameSpyServer = ( CGameSpyServer* )pUserData;
	pGameSpyServer->m_ePublishStatus = ePublishStatus_Error;
}

// RETAIL: 0x10003c40
void CGameSpyServer::NatNegProgressCallback( NegotiateState state, void* pUserData )
{
}

// RETAIL: 0x10003c50
void CGameSpyServer::NatNegCompletedCallback( NegotiateResult result, SOCKET gameSocket,
	sockaddr_in* pRemoteAddr, void* pUserData )
{
}

// RETAIL: 0x10003c70
DWORD WINAPI CGameSpyServer::ThreadProc( void* pParam )
{
	return (( CGameSpyServer* )pParam )->ThreadMain( );
}

// RETAIL: 0x10003c80. A client asks to negotiate through NAT with us.
void CGameSpyServer::NatNegCallback( int nCookie, void* pUserData )
{
	CGameSpyServer* pGameSpyServer = ( CGameSpyServer* )pUserData;

	CWinSync_CSAuto lockData( pGameSpyServer->m_csData );

	NNBeginNegotiationWithSocket(( SOCKET )pGameSpyServer->m_StartupInfo.m_hSocket, nCookie, 1,
		NatNegProgressCallback, NatNegCompletedCallback, pGameSpyServer );
}

// RETAIL: 0x10003cc0
uint8 CGameSpyServer::FindKey( char const* pszKeyName )
{
	CWinSync_CSAuto lockData( m_csData );

	uint8 nKeyId = 0;
	for( std::vector< std::string >::iterator iter = m_lstKeyNames.begin( ); iter != m_lstKeyNames.end( ); iter++, nKeyId++ )
	{
		if( !_stricmp( pszKeyName, iter->c_str( )))
			return nKeyId;
	}

	return GAMESPY_INVALID_KEYID;
}

// RETAIL: 0x10003d50
bool CGameSpyServer::RegisterKey( char const* pszKeyName )
{
	if( !pszKeyName || !pszKeyName[0] )
		return false;

	// Already registered?
	if( FindKey( pszKeyName ) != GAMESPY_INVALID_KEYID )
		return true;

	uint8 nKeyId = CGameSpyCommon::Instance( ).GetKeyId( pszKeyName, &m_nNextKeyId );
	if( nKeyId == GAMESPY_INVALID_KEYID )
		return false;

	CWinSync_CSAuto lockData( m_csData );

	m_lstKeyNames[nKeyId] = std::string( pszKeyName );
	qr2_register_key( nKeyId, pszKeyName );

	return true;
}

// RETAIL: 0x10003e10
void CGameSpyServer::ServerKeyCallback( int nKeyId, qr2_buffer_t outBuf, void* pUserData )
{
	CGameSpyServer* pGameSpyServer = ( CGameSpyServer* )pUserData;

	CWinSync_CSAuto lockData( pGameSpyServer->m_csData );

	char szValue[256];
	if( pGameSpyServer->GetServerProperty( pGameSpyServer->m_lstKeyNames[nKeyId].c_str( ), szValue, sizeof( szValue )))
		qr2_buffer_add( outBuf, szValue );
	else
		qr2_buffer_add( outBuf, "" );
}

// RETAIL: 0x10003ea0. The players' and the teams' keys are their key
// names with the index appended ("score_" -> "score_3").
void CGameSpyServer::PlayerTeamKeyCallback( int nKeyId, int nIndex, qr2_buffer_t outBuf, void* pUserData )
{
	CGameSpyServer* pGameSpyServer = ( CGameSpyServer* )pUserData;

	CWinSync_CSAuto lockData( pGameSpyServer->m_csData );

	char szKeyName[256];
	_snprintf( szKeyName, sizeof( szKeyName ), "%s%d", pGameSpyServer->m_lstKeyNames[nKeyId].c_str( ), nIndex );

	char szValue[256];
	if( pGameSpyServer->GetServerProperty( szKeyName, szValue, sizeof( szValue )))
		qr2_buffer_add( outBuf, szValue );
	else
		qr2_buffer_add( outBuf, "" );
}

// RETAIL: 0x10003f60. Server keys have no '_'; player keys end in '_';
// team keys have "_t".
void CGameSpyServer::KeyListCallback( qr2_key_type keyType, qr2_keybuffer_t keyBuffer, void* pUserData )
{
	CGameSpyServer* pGameSpyServer = ( CGameSpyServer* )pUserData;

	CWinSync_CSAuto lockData( pGameSpyServer->m_csData );

	for( uint8 nKeyId = 0; nKeyId < pGameSpyServer->m_lstKeyNames.size( ); nKeyId++ )
	{
		std::string& sKeyName = pGameSpyServer->m_lstKeyNames[nKeyId];
		if( sKeyName.empty( ))
			continue;

		switch( keyType )
		{
			case key_server:
				if( sKeyName.rfind( "_" ) == std::string::npos )
					qr2_keybuffer_add( keyBuffer, nKeyId );
				break;
			case key_player:
				if( sKeyName[sKeyName.length( ) - 1] == '_' )
					qr2_keybuffer_add( keyBuffer, nKeyId );
				break;
			case key_team:
				if( sKeyName.rfind( "_t" ) != std::string::npos )
					qr2_keybuffer_add( keyBuffer, nKeyId );
				break;
		}
	}
}

// RETAIL: 0x10004070
bool CGameSpyServer::Publish( )
{
	if( !Unpublish( ))
		return false;

	char szGameName[32];
	if( !CGameSpyCommon::Instance( ).GetQueryGameName( m_StartupInfo.m_nGameType, szGameName, sizeof( szGameName )))
		return false;

	char szSecretKey[32];
	if( !CGameSpyCommon::Instance( ).GetSecretKey( m_StartupInfo.m_nGameType, szSecretKey, sizeof( szSecretKey )))
		return false;

	SetServerProperty( "gsgamename", szGameName );

	// Share the game's socket if it has one.
	if( m_StartupInfo.m_hSocket )
	{
		if( qr2_init_socket( NULL, ( SOCKET )m_StartupInfo.m_hSocket, m_StartupInfo.m_nPort, szGameName, szSecretKey,
			m_StartupInfo.m_bPublic, 1, ServerKeyCallback, PlayerTeamKeyCallback, PlayerTeamKeyCallback,
			KeyListCallback, CountCallback, AddErrorCallback, this ) != e_qrnoerror )
			return false;
	}
	else
	{
		if( qr2_init( NULL, m_StartupInfo.m_sIP.c_str( ), m_StartupInfo.m_nPort + 1, szGameName, szSecretKey,
			m_StartupInfo.m_bPublic, 1, ServerKeyCallback, PlayerTeamKeyCallback, PlayerTeamKeyCallback,
			KeyListCallback, CountCallback, AddErrorCallback, this ) != e_qrnoerror )
			return false;
	}

	int nCDKeyGameId;
	if( CGameSpyCommon::Instance( ).GetCDKeyGameId( m_StartupInfo.m_nGameType, &nCDKeyGameId ))
	{
		gcd_init_qr2( NULL, nCDKeyGameId );
		m_bCDKeyChecks = true;
	}

	qr2_register_natneg_callback( NULL, NatNegCallback );

	DWORD dwThreadId;
	m_hThread = CreateThread( NULL, 0, ThreadProc, this, 0, &dwThreadId );
	WaitForSingleObject( m_ThreadStartedEvent.GetEvent( ), INFINITE );

	m_ePublishStatus = ePublishStatus_Published;
	return true;
}

// RETAIL: 0x100041f0
IGameSpyServer::EChallengeResponse CGameSpyServer::GetChallengeResponse( unsigned int nClientId )
{
	if( !m_bCDKeyChecks )
		return eChallengeResponse_Invalid;

	CWinSync_CSAuto lockData( m_csData );

	ChallengeResponseMap::iterator iter = m_mapChallengeResponses.find( nClientId );
	if( iter == m_mapChallengeResponses.end( ))
		return eChallengeResponse_Invalid;

	return iter->second;
}

// RETAIL: 0x10004260
void CGameSpyServer::AuthCallback( int nGameId, int nLocalId, int bAuthenticated, char* pszErrMsg, void* pInstance )
{
	CGameSpyServer* pGameSpyServer = ( CGameSpyServer* )pInstance;

	if( !pGameSpyServer->m_bCDKeyChecks )
		return;

	CWinSync_CSAuto lockData( pGameSpyServer->m_csData );

	ChallengeResponseMap::iterator iter = pGameSpyServer->m_mapChallengeResponses.find( nLocalId );
	if( iter == pGameSpyServer->m_mapChallengeResponses.end( ))
		return;

	iter->second = bAuthenticated ? eChallengeResponse_Accepted : eChallengeResponse_Rejected;
}

// RETAIL: 0x100042c0
bool CGameSpyServer::ReleaseCDKey( unsigned int nClientId )
{
	if( !m_bCDKeyChecks )
		return false;

	int nCDKeyGameId;
	if( !CGameSpyCommon::Instance( ).GetCDKeyGameId( m_StartupInfo.m_nGameType, &nCDKeyGameId ))
		return false;

	CWinSync_CSAuto lockData( m_csData );

	ChallengeResponseMap::iterator iter = m_mapChallengeResponses.find( nClientId );
	if( iter == m_mapChallengeResponses.end( ))
		return false;

	m_mapChallengeResponses.erase( iter );
	gcd_disconnect_user( nCDKeyGameId, nClientId );

	return true;
}

// RETAIL: 0x10004370
bool CGameSpyServer::GetServerProperty( char const* pszKeyName, char* pszValue, unsigned int nValueLen )
{
	if( !pszKeyName || !pszValue )
		return false;

	pszValue[0] = '\0';

	CWinSync_CSAuto lockData( m_csData );

	PropertyMap::iterator iter = m_mapProperties.find( pszKeyName );
	if( iter == m_mapProperties.end( ))
		return false;

	strncpy( pszValue, iter->second.c_str( ), nValueLen );
	pszValue[nValueLen-1] = '\0';
	return true;
}

// RETAIL: 0x10004430
bool CGameSpyServer::ProcessCDKeyChallengeResponse( unsigned int nClientId, char const* pszClientIP,
	char const* pszChallenge, char const* pszResponse )
{
	if( !m_bCDKeyChecks || !pszClientIP || !pszClientIP[0] || !pszChallenge || !pszResponse )
		return false;

	int nCDKeyGameId;
	if( !CGameSpyCommon::Instance( ).GetCDKeyGameId( m_StartupInfo.m_nGameType, &nCDKeyGameId ))
		return false;

	unsigned int nClientIP = inet_addr( pszClientIP );

	CWinSync_CSAuto lockData( &m_csData.GetCS( ));

	m_mapChallengeResponses[nClientId] = eChallengeResponse_Processing;
	gcd_authenticate_user( nCDKeyGameId, nClientId, nClientIP, pszChallenge, pszResponse, AuthCallback, this );

	return true;
}

// RETAIL: 0x100044f0
CGameSpyServer::~CGameSpyServer( )
{
	Unpublish( );
}

// RETAIL: 0x100045c0
bool CGameSpyServer::SetServerProperty( char const* pszKeyName, char const* pszValue )
{
	if( !pszKeyName || !pszValue )
		return false;

	CWinSync_CSAuto lockData( m_csData );

	m_mapProperties[pszKeyName] = pszValue;
	return true;
}

// RETAIL: 0x10004630
CGameSpyServer::CGameSpyServer( StartupInfo const& startupInfo )
{
	m_StartupInfo = startupInfo;
	m_ePublishStatus = ePublishStatus_Idle;
	m_nNextKeyId = GAMESPY_FIRST_CUSTOM_KEYID;
	m_lstKeyNames.resize( GAMESPY_MAX_KEYS );

	// The keys every server reports.
	RegisterKey( "gamever" );
	SetServerProperty( "gamever", m_StartupInfo.m_sVersion.c_str( ));
	RegisterKey( "numplayers" );
	RegisterKey( "numteams" );
	RegisterKey( "gsgamename" );

	m_nNumPlayers = 0;
	m_nNumTeams = 0;
	m_bCDKeyChecks = false;
	m_hThread = NULL;
}

// RETAIL: 0x100047e0
IGameSpyServer* IGameSpyServer::Create( StartupInfo const& startupInfo )
{
	return new CGameSpyServer( startupInfo );
}
