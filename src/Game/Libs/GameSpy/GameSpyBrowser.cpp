// ----------------------------------------------------------------------- //
//
// MODULE  : GameSpyBrowser.cpp
//
// PURPOSE : IGameSpyBrowser on GameSpy's server browsing SDK.
//
// cjacksrc: Monolith's source is lost; reconstructed from the retail
// GameSpy.dll (1.1), 0x10001000..0x10002f0b. The browser runs its SDKs on
// a thread of its own: the requests set up what the thread does, under two
// critical sections (the SDK's, then the request state's).
//
// ----------------------------------------------------------------------- //

#include "GameSpyBrowser.h"
#include "Ping.h"
#include "GameSpySDK/gcdkey/gcdkeyc.h"
#include "GameSpySDK/qr2/qr2regkeys.h"
#include "WONAPI/WONAuth/CDKey.h"

// The LAN search covers the ports around the game's.
#define LAN_SEARCH_PORTS_BELOW	50
#define LAN_SEARCH_PORTS_ABOVE	100

// A list request is done once the list has been quiet this long.
#define LIST_IDLE_TIMEOUT		3000

// The browser thread's beat.
#define THREAD_THINK_PERIOD		10

// How long a dying thread is waited for.
#define THREAD_END_TIMEOUT		1000

// The length of a CD key challenge response (gcd_compute_response's).
#define CDKEY_RESPONSE_LEN		73

// The first byte of NAT negotiation packets (NN_MAGIC_0).
#define NATNEG_PACKET_MAGIC		0xfd

// RETAIL: 0x100011d0
void IGameSpyBrowser::Delete( IGameSpyBrowser* pGameSpyBrowser )
{
	if( pGameSpyBrowser )
		delete pGameSpyBrowser;
}

// RETAIL: 0x100011e0
bool CGameSpyBrowser::RequestServerList( bool bInternet, unsigned short nPort, char const* pszFilter,
	ServerInfoCallback cbServerInfo, void* pUserData )
{
	if( !cbServerInfo )
		return false;

	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	if( m_eBrowserStatus == eBrowserStatus_Processing )
		return false;

	m_eBrowserStatus = eBrowserStatus_Error;

	if( !m_hServerBrowser )
		return false;

	if( !HaltRequest( ))
		return false;

	ServerBrowserClear( m_hServerBrowser );

	m_cbServerInfo = cbServerInfo;
	m_pUserData = pUserData;
	m_eRequestType = eRequestType_ServerList;
	m_dwRequestTime = GetTickCount( );
	m_eBrowserStatus = eBrowserStatus_Error;

	if( bInternet )
	{
		// The thread starts the update, with the filter.
		m_bStartListUpdate = true;
		m_sFilter = pszFilter ? pszFilter : "";
	}
	else
	{
		unsigned short nStartPort = ( unsigned short )LTMAX( nPort - LAN_SEARCH_PORTS_BELOW, 0 );
		unsigned short nEndPort = ( unsigned short )LTMIN( nStartPort + LAN_SEARCH_PORTS_ABOVE, 0xffff );

		if( ServerBrowserLANUpdate( m_hServerBrowser, SBTrue, nStartPort, nEndPort ) != sbe_noerror )
			return false;
	}

	m_eBrowserStatus = eBrowserStatus_Processing;
	return true;
}

// RETAIL: 0x100012f0
bool CGameSpyBrowser::RequestServerDetails( char const* pszIP, unsigned short nPort, bool bDirectConnect,
	ServerInfoCallback cbServerInfo, void* pUserData )
{
	if( !cbServerInfo || !pszIP || !pszIP[0] || !nPort )
		return false;

	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	if( m_eBrowserStatus == eBrowserStatus_Processing )
		return false;

	m_eBrowserStatus = eBrowserStatus_Error;

	if( !m_hServerBrowser || !HaltRequest( ))
		return false;

	ServerBrowserClear( m_hServerBrowser );

	m_cbServerInfo = cbServerInfo;
	m_pUserData = pUserData;
	m_eRequestType = eRequestType_ServerDetails;

	if( ServerBrowserAuxUpdateIP( m_hServerBrowser, pszIP, nPort, ( SBBool )!bDirectConnect, SBFalse, SBTrue ) != sbe_noerror )
	{
		m_eBrowserStatus = eBrowserStatus_Error;
		return false;
	}

	m_eBrowserStatus = eBrowserStatus_Complete;
	return true;
}

// RETAIL: 0x100013f0
IGameSpyBrowser::ENatNegotiationResult CGameSpyBrowser::GetNatNegotiationResult( sockaddr_in* pRemoteAddr ) const
{
	if( pRemoteAddr )
		*pRemoteAddr = m_NatNegotiationAddr;

	return m_eNatNegotiationResult;
}

// RETAIL: 0x10001420
bool CGameSpyBrowser::HaltRequest( )
{
	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	m_eBrowserStatus = eBrowserStatus_Idle;
	ServerBrowserHalt( m_hServerBrowser );
	NNFreeNegotiateList( );
	m_eRequestType = eRequestType_None;
	m_bStartListUpdate = false;

	return true;
}

// RETAIL: 0x10001470
int CGameSpyBrowser::RequestServerCount( bool bInternet, unsigned short nPort )
{
	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	if( m_eBrowserStatus == eBrowserStatus_Processing )
		return -1;

	m_eBrowserStatus = eBrowserStatus_Error;

	if( !m_hServerBrowser )
		return -1;

	if( !HaltRequest( ))
		return -1;

	m_cbServerInfo = NULL;
	m_pUserData = NULL;
	ServerBrowserClear( m_hServerBrowser );

	if( bInternet )
	{
		if( ServerBrowserUpdate( m_hServerBrowser, SBFalse, SBFalse, NULL, 0, NULL ) != sbe_noerror )
			return -1;
	}
	else
	{
		unsigned short nStartPort = ( unsigned short )LTMAX( nPort - LAN_SEARCH_PORTS_BELOW, 0 );
		unsigned short nEndPort = ( unsigned short )LTMIN( nStartPort + LAN_SEARCH_PORTS_ABOVE, 0xffff );

		if( ServerBrowserLANUpdate( m_hServerBrowser, SBFalse, nStartPort, nEndPort ) != sbe_noerror )
			return -1;
	}

	m_eBrowserStatus = eBrowserStatus_Complete;
	return ServerBrowserCount( m_hServerBrowser );
}

// RETAIL: 0x10001580
int CGameSpyBrowser::GetNumPlayers( HGAMESERVER hGameServer )
{
	if( !m_hServerBrowser )
		return 0;

	char szValue[256];
	if( !GetServerProperty( hGameServer, "numplayers", szValue, sizeof( szValue )))
		return 0;

	return atoi( szValue );
}

// RETAIL: 0x100015d0
int CGameSpyBrowser::GetNumTeams( HGAMESERVER hGameServer )
{
	if( !m_hServerBrowser )
		return 0;

	char szValue[256];
	if( !GetServerProperty( hGameServer, "numteams", szValue, sizeof( szValue )))
		return 0;

	return atoi( szValue );
}

// RETAIL: 0x10001620
bool CGameSpyBrowser::GetServerProperty( HGAMESERVER hGameServer, char const* pszKeyName,
	char* pszValue, unsigned int nValueLen )
{
	if( !pszKeyName || !pszKeyName[0] || !pszValue || !hGameServer || !m_hServerBrowser )
		return false;

	CWinSync_CSAuto lockServerBrowser( &m_csServerBrowser.GetCS( ));
	CWinSync_CSAuto lockData( &m_csData.GetCS( ));

	char const* pszServerValue = SBServerGetStringValue(( SBServer )hGameServer, pszKeyName, NULL );
	if( !pszServerValue )
		return false;

	strncpy( pszValue, pszServerValue, nValueLen );
	pszValue[nValueLen-1] = '\0';
	return true;
}

// RETAIL: 0x100016d0. An ICMP ping if it can, else the SDK's own.
unsigned short CGameSpyBrowser::GetServerPing( HGAMESERVER hGameServer )
{
	if( !m_hServerBrowser )
		return 0;

	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	char szAddress[16];
	strcpy( szAddress, SBServerGetPublicAddress(( SBServer )hGameServer ));

	unsigned short nPing;
	unsigned short nICMPPing = Ping( szAddress );
	if( nICMPPing != 0xffff )
	{
		nPing = nICMPPing;
	}
	else
	{
		int nServerPing = SBServerGetPing(( SBServer )hGameServer );
		if( !nServerPing )
			nPing = 0xffff;
		else if( nServerPing < 0xfffe )
			nPing = nServerPing;
		else
			nPing = 0xfffe;
	}

	return nPing;
}

// RETAIL: 0x10001770
bool CGameSpyBrowser::HasServerDetails( HGAMESERVER hGameServer )
{
	if( !m_hServerBrowser )
		return false;

	if( !hGameServer )
		return false;

	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	return ( SBServerHasFullKeys(( SBServer )hGameServer ) == SBTrue );
}

// RETAIL: 0x100017d0
bool CGameSpyBrowser::GetServerConnectInfo( HGAMESERVER hGameServer,
	char* pszPrivateIP, unsigned short* pnPrivatePort,
	char* pszPublicIP, unsigned short* pnPublicPort,
	bool* pbUsePublicAddress, bool* pbDirectConnect )
{
	pszPrivateIP[0] = '\0';
	pszPublicIP[0] = '\0';
	*pnPrivatePort = 0;
	*pnPublicPort = 0;
	*pbUsePublicAddress = true;
	*pbDirectConnect = true;

	if( !m_hServerBrowser )
		return false;

	SBServer server = ( SBServer )hGameServer;
	if( !server )
		return false;

	strcpy( pszPublicIP, SBServerGetPublicAddress( server ));
	*pnPublicPort = SBServerGetPublicQueryPort( server );

	if( SBServerHasPrivateAddress( server ) == SBTrue )
	{
		strcpy( pszPrivateIP, SBServerGetPrivateAddress( server ));
		*pnPrivatePort = SBServerGetPrivateQueryPort( server );

		// Behind the same NAT as us: connect to its private address.
		if( SBServerGetPublicInetAddress( server ) == ServerBrowserGetMyPublicIPAddr( m_hServerBrowser ))
		{
			*pbUsePublicAddress = false;
			*pbDirectConnect = true;
			return true;
		}
	}
	else
	{
		strcpy( pszPrivateIP, pszPublicIP );
		*pnPrivatePort = *pnPublicPort;
	}

	strcpy( pszPublicIP, SBServerGetPublicAddress( server ));
	*pnPublicPort = SBServerGetPublicQueryPort( server );
	*pbDirectConnect = ( SBServerDirectConnect( server ) == SBTrue );
	return true;
}

// RETAIL: 0x10001910
void CGameSpyBrowser::SBCallback( ServerBrowser sb, SBCallbackReason reason, SBServer server, void* pInstance )
{
	CGameSpyBrowser* pGameSpyBrowser = ( CGameSpyBrowser* )pInstance;
	bool bOurGame = false;
	char szGameName[256];

	CWinSync_CSAuto lockData( pGameSpyBrowser->m_csData );

	pGameSpyBrowser->m_dwRequestTime = GetTickCount( );

	switch( reason )
	{
		case sbc_serveradded:
		case sbc_serverupdated:
			// Only report servers of our game.
			if( !pGameSpyBrowser->GetServerProperty( server, "gsgamename", szGameName, sizeof( szGameName )))
				break;
			bOurGame = ( _stricmp( szGameName, pGameSpyBrowser->m_sGameName.c_str( )) == 0 );
			if( !bOurGame )
				break;

			if( pGameSpyBrowser->m_cbServerInfo )
				pGameSpyBrowser->m_cbServerInfo( *pGameSpyBrowser, server, pGameSpyBrowser->m_pUserData );
			break;
		case sbc_updatecomplete:
			if( pGameSpyBrowser->m_eBrowserStatus == eBrowserStatus_Processing )
				pGameSpyBrowser->m_eBrowserStatus = eBrowserStatus_Complete;
			break;
		case sbc_serverupdatefailed:
		case sbc_queryerror:
			pGameSpyBrowser->m_eBrowserStatus = eBrowserStatus_Error;
			break;
		case sbc_serverdeleted:
			break;
	}
}

// RETAIL: 0x10001a20
void CGameSpyBrowser::NatNegProgressCallback( NegotiateState state, void* pUserData )
{
}

// RETAIL: 0x10001a30
void CGameSpyBrowser::NatNegCompletedCallback( NegotiateResult result, SOCKET gameSocket,
	sockaddr_in* pRemoteAddr, void* pUserData )
{
	CGameSpyBrowser* pGameSpyBrowser = ( CGameSpyBrowser* )pUserData;

	CWinSync_CSAuto lockData( pGameSpyBrowser->m_csData );

	switch( result )
	{
		case nr_success:
			pGameSpyBrowser->m_NatNegotiationAddr = *pRemoteAddr;
			pGameSpyBrowser->m_eNatNegotiationResult = eNatNegotiationResult_Success;
			pGameSpyBrowser->m_eBrowserStatus = eBrowserStatus_Complete;
			break;
		case nr_inittimeout:
			pGameSpyBrowser->m_eBrowserStatus = eBrowserStatus_Error;
			pGameSpyBrowser->m_eNatNegotiationResult = eNatNegotiationResult_Timeout;
			break;
		default:
			pGameSpyBrowser->m_eBrowserStatus = eBrowserStatus_Error;
			pGameSpyBrowser->m_eNatNegotiationResult = eNatNegotiationResult_Failed;
			break;
	}
}

// RETAIL: 0x10001ab0
bool CGameSpyBrowser::GetCDKeyChallengeResponse( char const* pszCDKey, char const* pszChallenge,
	char* pszResponse, unsigned int* pnResponseLen )
{
	if( !pszCDKey || !pszCDKey[0] || !pszChallenge || !pszChallenge[0] )
		return false;

	unsigned int nResponseLen = *pnResponseLen;
	*pnResponseLen = CDKEY_RESPONSE_LEN;

	// Just asking for the length?
	if( !pszResponse )
		return true;

	if( nResponseLen < CDKEY_RESPONSE_LEN )
		return false;

	char szResponse[CDKEY_RESPONSE_LEN];
	gcd_compute_response(( char* )pszCDKey, ( char* )pszChallenge, szResponse );
	strncpy( pszResponse, szResponse, nResponseLen );
	return true;
}

// RETAIL: 0x10001b20
bool CGameSpyBrowser::HandleNetMessage( unsigned char const* pData, unsigned int nDataLen,
	unsigned char const* pSenderAddr, unsigned short nSenderPort )
{
	if( nDataLen < 1 || pData[0] != NATNEG_PACKET_MAGIC )
		return false;

	sockaddr_in senderAddr;
	memset( &senderAddr, 0, sizeof( senderAddr ));
	senderAddr.sin_family = AF_INET;
	unsigned long nSenderAddr = ( pSenderAddr[0] << 24 ) | ( pSenderAddr[1] << 16 ) | ( pSenderAddr[2] << 8 ) | pSenderAddr[3];
	senderAddr.sin_addr.s_addr = htonl( nSenderAddr );
	senderAddr.sin_port = htons( nSenderPort );

	NNProcessData(( char* )pData, nDataLen, &senderAddr );
	return true;
}

// RETAIL: 0x10001bb0
bool CGameSpyBrowser::RequestNatNegotiation( unsigned int hSocket, char const* pszIP, unsigned short nPort )
{
	if( !pszIP || !pszIP[0] || !nPort )
		return false;

	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	if( m_eBrowserStatus == eBrowserStatus_Processing )
		return false;

	m_eNatNegotiationResult = eNatNegotiationResult_Failed;
	m_eBrowserStatus = eBrowserStatus_Error;

	if( !m_hServerBrowser || !HaltRequest( ))
		return false;

	ServerBrowserClear( m_hServerBrowser );
	m_eRequestType = eRequestType_NatNegotiation;

	// Both sides negotiate with the same cookie.
	int nCookie = rand( );
	if( ServerBrowserSendNatNegotiateCookieToServer( m_hServerBrowser, pszIP, nPort, nCookie ) != sbe_noerror )
		return false;

	if( NNBeginNegotiationWithSocket(( SOCKET )hSocket, nCookie, 0, NatNegProgressCallback,
		NatNegCompletedCallback, this ) != ne_noerror )
		return false;

	m_eBrowserStatus = eBrowserStatus_Processing;
	return true;
}

// RETAIL: 0x10001ca0
uint8 CGameSpyBrowser::FindKey( char const* pszKeyName )
{
	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	uint8 nKeyId = 0;
	for( std::vector< std::string >::iterator iter = m_lstKeyNames.begin( ); iter != m_lstKeyNames.end( ); iter++, nKeyId++ )
	{
		if( !_stricmp( pszKeyName, iter->c_str( )))
			return nKeyId;
	}

	return GAMESPY_INVALID_KEYID;
}

// RETAIL: 0x10001d40
bool IsCDKeyValid( char const* pszCDKey, int nGameType )
{
	if( !pszCDKey || !pszCDKey[0] )
		return false;

	char szGameName[32];
	if( !CGameSpyCommon::Instance( ).GetReportGameName( nGameType, szGameName, sizeof( szGameName )))
		return false;

	// The key's checksum covers the product.
	WONAPI::CDKey cdKey;
	std::string sGameName = szGameName;
	cdKey.SetProductString( sGameName );
	std::string sCDKey = pszCDKey;
	bool bValid = cdKey.Init( pszCDKey );

	return bValid;
}

// RETAIL: 0x10001e20
bool CGameSpyBrowser::RegisterKey( char const* pszKeyName )
{
	if( !pszKeyName || !pszKeyName[0] )
		return false;

	// Already registered?
	if( FindKey( pszKeyName ) != GAMESPY_INVALID_KEYID )
		return true;

	uint8 nKeyId = CGameSpyCommon::Instance( ).GetKeyId( pszKeyName, &m_nNextKeyId );
	if( nKeyId == GAMESPY_INVALID_KEYID )
		return false;

	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	m_lstKeyNames[nKeyId] = std::string( pszKeyName );
	qr2_register_key( nKeyId, pszKeyName );

	return true;
}

// RETAIL: 0x10001ef0
bool CGameSpyBrowser::IsCDKeyValid( char const* pszCDKey )
{
	return ::IsCDKeyValid( pszCDKey, m_StartupInfo.m_nGameType );
}

// RETAIL: 0x10001f10
DWORD CGameSpyBrowser::ThreadMain( )
{
	m_ThreadStartedEvent.Set( );

	do
	{
		CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );

		// Start an internet list update.
		if( m_bStartListUpdate )
		{
			m_bStartListUpdate = false;
			if( ServerBrowserUpdate( m_hServerBrowser, SBTrue, SBFalse, &m_lstSummaryKeys[0],
				m_lstSummaryKeys.size( ), m_sFilter.c_str( )) != sbe_noerror )
			{
				m_eBrowserStatus = eBrowserStatus_Error;
			}
		}

		ServerBrowserThink( m_hServerBrowser );

		{
			CWinSync_CSAuto lockData( m_csData );

			if( m_eBrowserStatus == eBrowserStatus_Processing )
			{
				if( ServerBrowserState( m_hServerBrowser ) == sb_disconnected )
					m_eBrowserStatus = eBrowserStatus_Complete;

				if( m_eRequestType == eRequestType_ServerList )
				{
					// Done when every server has answered...
					int nNumServers = ServerBrowserCount( m_hServerBrowser );
					if( nNumServers > 0 )
					{
						int nNumPending = ServerBrowserPendingQueryCount( m_hServerBrowser );
						if((( nNumServers - nNumPending ) * 100 ) / nNumServers == 100 )
							m_eBrowserStatus = eBrowserStatus_Complete;
					}

					// ...or the list has been quiet for a while.
					if( GetTickCount( ) - m_dwRequestTime > LIST_IDLE_TIMEOUT )
						m_eBrowserStatus = eBrowserStatus_Complete;
				}
			}
		}

		NNThink( );
	}
	while( WaitForSingleObject( m_ThreadEndEvent.GetEvent( ), THREAD_THINK_PERIOD ) == WAIT_TIMEOUT );

	return 0;
}

// RETAIL: 0x10002050
DWORD WINAPI CGameSpyBrowser::ThreadProc( void* pParam )
{
	return (( CGameSpyBrowser* )pParam )->ThreadMain( );
}

// RETAIL: 0x10002060
bool CGameSpyBrowser::AddSummaryKey( char const* pszKeyName )
{
	if( !pszKeyName || !pszKeyName[0] )
		return false;

	CWinSync_CSAuto lockServerBrowser( m_csServerBrowser );
	CWinSync_CSAuto lockData( m_csData );

	uint8 nKeyId = FindKey( pszKeyName );
	if( nKeyId == GAMESPY_INVALID_KEYID )
		return false;

	m_lstSummaryKeys.push_back( nKeyId );
	return true;
}

// RETAIL: 0x100020e0
CGameSpyBrowser::~CGameSpyBrowser( )
{
	if( m_hThread )
	{
		m_ThreadEndEvent.Set( );
		if( WaitForSingleObject( m_hThread, THREAD_END_TIMEOUT ) == WAIT_TIMEOUT )
			TerminateThread( m_hThread, 0 );
		CloseHandle( m_hThread );
		m_hThread = NULL;
	}

	NNFreeNegotiateList( );

	if( m_hServerBrowser )
	{
		ServerBrowserFree( m_hServerBrowser );
		m_hServerBrowser = NULL;
	}
}

// RETAIL: 0x100021b0
CGameSpyBrowser::CGameSpyBrowser( StartupInfo const& startupInfo )
{
#ifdef CJACK_MODERN
	// Set before anything can fail, for the destructor (RETAIL_BUGS.md).
	m_hThread = NULL;
	m_hServerBrowser = NULL;
#endif
	m_StartupInfo = startupInfo;
	m_eBrowserStatus = eBrowserStatus_Idle;
	m_cbServerInfo = NULL;
	m_pUserData = NULL;
	m_eNatNegotiationResult = eNatNegotiationResult_None;
	m_nNextKeyId = GAMESPY_FIRST_CUSTOM_KEYID;
	m_lstKeyNames.resize( GAMESPY_MAX_KEYS );
	m_eRequestType = eRequestType_None;
	m_bStartListUpdate = false;
	m_dwRequestTime = 0;

	// The keys the list always carries.
	RegisterKey( "gamever" );
	RegisterKey( "numplayers" );
	RegisterKey( "numteams" );
	RegisterKey( "gsgamename" );
	AddSummaryKey( "gsgamename" );

	char szQueryGameName[32];
	if( !CGameSpyCommon::Instance( ).GetQueryGameName( m_StartupInfo.m_nGameType, szQueryGameName, sizeof( szQueryGameName )))
		return;

	char szReportGameName[32];
	if( !CGameSpyCommon::Instance( ).GetReportGameName( m_StartupInfo.m_nGameType, szReportGameName, sizeof( szReportGameName )))
		return;

	char szSecretKey[32];
	if( !CGameSpyCommon::Instance( ).GetSecretKey( m_StartupInfo.m_nGameType, szSecretKey, sizeof( szSecretKey )))
		return;

	m_sGameName = szQueryGameName;

	m_hServerBrowser = ServerBrowserNew( szQueryGameName, szReportGameName, szSecretKey, 0, 10,
		QVERSION_QR2, SBCallback, this );

	DWORD dwThreadId;
	m_hThread = CreateThread( NULL, 0, ThreadProc, this, 0, &dwThreadId );
	WaitForSingleObject( m_ThreadStartedEvent.GetEvent( ), INFINITE );
}

// RETAIL: 0x10002360
IGameSpyBrowser* IGameSpyBrowser::Create( StartupInfo const& startupInfo )
{
	return new CGameSpyBrowser( startupInfo );
}
