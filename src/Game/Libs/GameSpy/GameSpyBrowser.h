// ----------------------------------------------------------------------- //
//
// MODULE  : GameSpyBrowser.h
//
// PURPOSE : IGameSpyBrowser on GameSpy's server browsing SDK.
//
// cjacksrc: reconstructed from the retail GameSpy.dll (1.1); the layout is
// the retail object's (0xd0 bytes), the vtable 0x100162d8.
//
// ----------------------------------------------------------------------- //

#ifndef __GAMESPYBROWSER_H__
#define __GAMESPYBROWSER_H__

#include <vector>
#include <string>
#include "IGameSpy.h"
#include "GameSpyCommon.h"
#include "WinSync.h"
#include "GameSpySDK/serverbrowsing/sb_serverbrowsing.h"
#include "GameSpySDK/natneg/natneg.h"

class CGameSpyBrowser : public IGameSpyBrowser
{
	public:

		CGameSpyBrowser( StartupInfo const& startupInfo );
		virtual ~CGameSpyBrowser( );

		virtual bool RequestServerList( bool bInternet, unsigned short nPort, char const* pszFilter,
			ServerInfoCallback cbServerInfo, void* pUserData );
		virtual bool RequestServerDetails( char const* pszIP, unsigned short nPort, bool bDirectConnect,
			ServerInfoCallback cbServerInfo, void* pUserData );
		virtual bool RequestNatNegotiation( unsigned int hSocket, char const* pszIP, unsigned short nPort );
		virtual bool HaltRequest( );
		virtual EBrowserStatus GetBrowserStatus( ) const { return m_eBrowserStatus; }
		virtual ENatNegotiationResult GetNatNegotiationResult( sockaddr_in* pRemoteAddr ) const;
		virtual bool RegisterKey( char const* pszKeyName );
		virtual bool AddSummaryKey( char const* pszKeyName );
		virtual int RequestServerCount( bool bInternet, unsigned short nPort );

		virtual int GetNumPlayers( HGAMESERVER hGameServer );
		virtual int GetNumTeams( HGAMESERVER hGameServer );
		virtual bool GetServerProperty( HGAMESERVER hGameServer, char const* pszKeyName,
			char* pszValue, unsigned int nValueLen );
		virtual unsigned short GetServerPing( HGAMESERVER hGameServer );
		virtual bool HasServerDetails( HGAMESERVER hGameServer );
		virtual bool GetServerConnectInfo( HGAMESERVER hGameServer,
			char* pszPrivateIP, unsigned short* pnPrivatePort,
			char* pszPublicIP, unsigned short* pnPublicPort,
			bool* pbUsePublicAddress, bool* pbDirectConnect );

		virtual bool GetCDKeyChallengeResponse( char const* pszCDKey, char const* pszChallenge,
			char* pszResponse, unsigned int* pnResponseLen );
		virtual bool IsCDKeyValid( char const* pszCDKey );

		virtual bool HandleNetMessage( unsigned char const* pData, unsigned int nDataLen,
			unsigned char const* pSenderAddr, unsigned short nSenderPort );

	private:

		// What the current request is.
		enum ERequestType
		{
			eRequestType_None,
			eRequestType_ServerList,
			eRequestType_ServerDetails,
			eRequestType_NatNegotiation,
		};

		// The key id of a registered key name, or GAMESPY_INVALID_KEYID.
		uint8 FindKey( char const* pszKeyName );

		// The browser's thread: thinks the SDKs every 10 ms.
		static DWORD WINAPI ThreadProc( void* pParam );
		DWORD ThreadMain( );

		// The SDKs' callbacks.
		static void SBCallback( ServerBrowser sb, SBCallbackReason reason, SBServer server, void* pInstance );
		static void NatNegProgressCallback( NegotiateState state, void* pUserData );
		static void NatNegCompletedCallback( NegotiateResult result, SOCKET gameSocket,
			sockaddr_in* pRemoteAddr, void* pUserData );

	private:

		StartupInfo				m_StartupInfo;			// 0x04
		HANDLE					m_hThread;				// 0x08
		CWinSync_Event			m_ThreadEndEvent;		// 0x0c
		CWinSync_PulseEvent		m_ThreadStartedEvent;	// 0x10
		CWinSync_CS				m_csServerBrowser;		// 0x14: the SDKs
		std::vector< std::string >	m_lstKeyNames;		// 0x2c: by key id
		std::vector< uint8 >	m_lstSummaryKeys;		// 0x3c: the server list's keys
		uint8					m_nNextKeyId;			// 0x4c
		CWinSync_CS				m_csData;				// 0x50: the request's state
		EBrowserStatus			m_eBrowserStatus;		// 0x68
		ServerBrowser			m_hServerBrowser;		// 0x6c
		ServerInfoCallback		m_cbServerInfo;			// 0x70
		ENatNegotiationResult	m_eNatNegotiationResult;	// 0x74
		ERequestType			m_eRequestType;			// 0x78
		DWORD					m_dwRequestTime;		// 0x7c: last list activity
		sockaddr_in				m_NatNegotiationAddr;	// 0x80
		void*					m_pUserData;			// 0x90
		std::string				m_sGameName;			// 0x94: servers of other games are ignored
		bool					m_bStartListUpdate;		// 0xb0: the thread starts the internet list
		std::string				m_sFilter;				// 0xb4
};

#endif // __GAMESPYBROWSER_H__
