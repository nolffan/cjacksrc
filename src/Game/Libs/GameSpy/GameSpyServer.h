// ----------------------------------------------------------------------- //
//
// MODULE  : GameSpyServer.h
//
// PURPOSE : IGameSpyServer on GameSpy's query and reporting 2, NAT
//           negotiation and CD key SDKs.
//
// cjacksrc: reconstructed from the retail GameSpy.dll (1.1); the layout is
// the retail object's (0xe4 bytes), the vtable 0x10016504.
//
// ----------------------------------------------------------------------- //

#ifndef __GAMESPYSERVER_H__
#define __GAMESPYSERVER_H__

#include <string>
#include <vector>
#include <map>
#include <hash_map>
#include "IGameSpy.h"
#include "GameSpyCommon.h"
#include "WinSync.h"
#include "GameSpySDK/qr2/qr2.h"
#include "GameSpySDK/natneg/natneg.h"

// Hashes and orders the server's property names without regard to case.
class CPropertyNameHashCompare
{
	public:

		enum
		{
			bucket_size = 4,
			min_buckets = 8,
		};

		// RETAIL: 0x10003890
		size_t operator()( std::string const& sKey ) const
		{
			size_t nHash = 0;
			for( char const* pszKey = sKey.c_str( ); *pszKey; pszKey++ )
				nHash = nHash * 5 + tolower( *pszKey );
			return nHash;
		}

		bool operator()( std::string const& sKey1, std::string const& sKey2 ) const
		{
			return ( _stricmp( sKey1.c_str( ), sKey2.c_str( )) < 0 );
		}
};

class CGameSpyServer : public IGameSpyServer
{
	public:

		CGameSpyServer( StartupInfo const& startupInfo );
		virtual ~CGameSpyServer( );

		virtual bool Publish( );
		virtual bool Unpublish( );
		virtual EPublishStatus GetPublishStatus( ) const { return m_ePublishStatus; }

		virtual bool RegisterKey( char const* pszKeyName );
		virtual bool GetServerProperty( char const* pszKeyName, char* pszValue, unsigned int nValueLen );
		virtual bool SetServerProperty( char const* pszKeyName, char const* pszValue );
		virtual void SetNumPlayers( int nNumPlayers );
		virtual void SetNumTeams( int nNumTeams );

		virtual bool GetCDKeyChallenge( char* pszChallenge, unsigned int* pnChallengeLen );
		virtual bool ProcessCDKeyChallengeResponse( int nClientId, char const* pszClientIP,
			char const* pszChallenge, char const* pszResponse );
		virtual EChallengeResponse GetChallengeResponse( int nClientId );
		virtual bool ReleaseCDKey( int nClientId );

		virtual bool HandleNetMessage( unsigned char const* pData, unsigned int nDataLen,
			unsigned char const* pSenderAddr, unsigned short nSenderPort );

	private:

		typedef stdext::hash_map< std::string, std::string, CPropertyNameHashCompare > PropertyMap;
		typedef std::map< int, EChallengeResponse > ChallengeResponseMap;

		// The key id of a registered key name, or GAMESPY_INVALID_KEYID.
		uint8 FindKey( char const* pszKeyName );

		// The server's thread: thinks the SDKs every 10 ms.
		static DWORD WINAPI ThreadProc( void* pParam );
		DWORD ThreadMain( );

		// The SDKs' callbacks.
		static void ServerKeyCallback( int nKeyId, qr2_buffer_t outBuf, void* pUserData );
		static void PlayerTeamKeyCallback( int nKeyId, int nIndex, qr2_buffer_t outBuf, void* pUserData );
		static void KeyListCallback( qr2_key_type keyType, qr2_keybuffer_t keyBuffer, void* pUserData );
		static int CountCallback( qr2_key_type keyType, void* pUserData );
		static void AddErrorCallback( qr2_error_t error, char* pszErrMsg, void* pUserData );
		static void NatNegProgressCallback( NegotiateState state, void* pUserData );
		static void NatNegCompletedCallback( NegotiateResult result, SOCKET gameSocket,
			sockaddr_in* pRemoteAddr, void* pUserData );
		static void NatNegCallback( int nCookie, void* pUserData );
		static void AuthCallback( int nGameId, int nLocalId, int bAuthenticated, char* pszErrMsg, void* pInstance );

	private:

		StartupInfo				m_StartupInfo;			// 0x04
		bool					m_bCDKeyChecks;			// 0x68
		HANDLE					m_hThread;				// 0x6c
		CWinSync_Event			m_ThreadEndEvent;		// 0x70
		CWinSync_PulseEvent		m_ThreadStartedEvent;	// 0x74
		PropertyMap				m_mapProperties;		// 0x78
		std::vector< std::string >	m_lstKeyNames;		// 0xa0: by key id
		uint8					m_nNextKeyId;			// 0xb0
		int						m_nNumPlayers;			// 0xb4
		int						m_nNumTeams;			// 0xb8
		CWinSync_CS				m_csData;				// 0xbc
		EPublishStatus			m_ePublishStatus;		// 0xd4
		ChallengeResponseMap	m_mapChallengeResponses;	// 0xd8: by client id
};

#endif // __GAMESPYSERVER_H__
