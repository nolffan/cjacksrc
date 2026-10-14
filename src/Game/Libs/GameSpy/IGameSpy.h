// ----------------------------------------------------------------------- //
//
// MODULE  : IGameSpy.h
//
// PURPOSE : The interfaces GameSpy.dll exports to the game: the server
//           browser (CShell.dll), the patch information request (CShell.dll)
//           and the published server with its CD key checks (Object.lto).
//
// cjacksrc: Monolith's header is lost; this one is reconstructed from the
// retail GameSpy.dll (1.1) and the calls CShell.dll and Object.lto make
// into it. The virtual functions are in the retail vtables' order
// (0x10016238, 0x10016284, 0x10016200); the names follow FEAR's use of its
// later version of these interfaces where one exists. The game includes
// this header from Game/Libs/GameSpy and links GameSpy.lib.
//
// ----------------------------------------------------------------------- //

#ifndef __IGAMESPY_H__
#define __IGAMESPY_H__

#include <string>

#ifdef GAMESPY_EXPORTS
#define GAMESPY_API __declspec(dllexport)
#else
#define GAMESPY_API __declspec(dllimport)
#endif

struct sockaddr_in;

// Which build of the game is talking to GameSpy: picks the game names, the
// secret key and the product ids (GameSpyCommon.cpp). The retail game DLLs
// pass 1.
enum EGameSpyGameType
{
	eGameSpyGameType_Retail		= 1,	// "contractjack"
	eGameSpyGameType_Demo		= 2,	// "contractjackd"
	eGameSpyGameType_Other		= 3,	// a product id only (patch checks)
	eGameSpyGameType_Preview	= 4,	// queries "contractjackd", reports "contractjackpr"
};

// Is a CD key well formed for this build of the game? (no network)
extern "C" GAMESPY_API bool IsCDKeyValid( char const* pszCDKey, int nGameType );


// ----------------------------------------------------------------------- //
// The server browser (CShell.dll)

class GAMESPY_API IGameSpyBrowser
{
	public:

		struct StartupInfo
		{
			StartupInfo( )
			{
				m_nGameType = 0;
			}
			int		m_nGameType;
		};

		enum EBrowserStatus
		{
			eBrowserStatus_Idle,
			eBrowserStatus_Processing,
			eBrowserStatus_Complete,
			eBrowserStatus_Error,
		};

		enum ENatNegotiationResult
		{
			eNatNegotiationResult_None,
			eNatNegotiationResult_Failed,
			eNatNegotiationResult_Timeout,
			eNatNegotiationResult_Success,
		};

		// A server of the list, valid while the browser has its list.
		typedef void* HGAMESERVER;

		// Called on the browser's thread for each server found or updated.
		typedef void ( *ServerInfoCallback )( IGameSpyBrowser& gameSpyBrowser,
			HGAMESERVER hGameServer, void* pUserData );

		static IGameSpyBrowser* Create( StartupInfo const& startupInfo );
		static void Delete( IGameSpyBrowser* pGameSpyBrowser );

		IGameSpyBrowser( ) { }
		virtual ~IGameSpyBrowser( ) { }

		// Request the list of servers, on the internet (with an optional
		// filter) or on the LAN (searching the ports around nPort).
		virtual bool RequestServerList( bool bInternet, unsigned short nPort, char const* pszFilter,
			ServerInfoCallback cbServerInfo, void* pUserData ) = 0;
		// Request the details of one server.
		virtual bool RequestServerDetails( char const* pszIP, unsigned short nPort, bool bDirectConnect,
			ServerInfoCallback cbServerInfo, void* pUserData ) = 0;
		// Negotiate a connection through NAT to a server, on hSocket.
		virtual bool RequestNatNegotiation( unsigned int hSocket, char const* pszIP, unsigned short nPort ) = 0;
		virtual bool HaltRequest( ) = 0;
		virtual EBrowserStatus GetBrowserStatus( ) const = 0;
		virtual ENatNegotiationResult GetNatNegotiationResult( sockaddr_in* pRemoteAddr ) const = 0;
		// Keys the server list carries.
		virtual bool RegisterKey( char const* pszKeyName ) = 0;
		virtual bool AddSummaryKey( char const* pszKeyName ) = 0;
		// Request the list and wait for it; returns the number of servers,
		// or -1.
		virtual int RequestServerCount( bool bInternet, unsigned short nPort ) = 0;

		virtual int GetNumPlayers( HGAMESERVER hGameServer ) = 0;
		virtual int GetNumTeams( HGAMESERVER hGameServer ) = 0;
		virtual bool GetServerProperty( HGAMESERVER hGameServer, char const* pszKeyName,
			char* pszValue, unsigned int nValueLen ) = 0;
		virtual unsigned short GetServerPing( HGAMESERVER hGameServer ) = 0;
		virtual bool HasServerDetails( HGAMESERVER hGameServer ) = 0;
		virtual bool GetServerConnectInfo( HGAMESERVER hGameServer,
			char* pszPrivateIP, unsigned short* pnPrivatePort,
			char* pszPublicIP, unsigned short* pnPublicPort,
			bool* pbUsePublicAddress, bool* pbDirectConnect ) = 0;

		// The client's answer to a server's CD key challenge.
		virtual bool GetCDKeyChallengeResponse( char const* pszCDKey, char const* pszChallenge,
			char* pszResponse, unsigned int* pnResponseLen ) = 0;
		virtual bool IsCDKeyValid( char const* pszCDKey ) = 0;

		// A packet the game's socket received that may be GameSpy's.
		virtual bool HandleNetMessage( unsigned char const* pData, unsigned int nDataLen,
			unsigned char const* pSenderAddr, unsigned short nSenderPort ) = 0;

	private:

		IGameSpyBrowser( IGameSpyBrowser const& );
		IGameSpyBrowser& operator=( IGameSpyBrowser const& );
};


// ----------------------------------------------------------------------- //
// The patch information request (CShell.dll)

class GAMESPY_API IGameSpyPatchInfo
{
	public:

		struct StartupInfo
		{
			StartupInfo( )
			{
				m_nGameType = 0;
				m_nDistributionId = 0;
			}
			int				m_nGameType;
			std::string		m_sVersion;
			int				m_nDistributionId;
		};

		enum EBrowserStatus
		{
			eBrowserStatus_Idle,
			eBrowserStatus_Processing,
			eBrowserStatus_Complete,
			eBrowserStatus_Error,
		};

		struct PatchInfoResults
		{
			bool			m_bNewVersion;
			bool			m_bMandatory;
			std::string		m_sVersionName;
			std::string		m_sURL;
		};

		static IGameSpyPatchInfo* Create( StartupInfo const& startupInfo );
		static void Delete( IGameSpyPatchInfo* pGameSpyPatchInfo );

		IGameSpyPatchInfo( ) { }
		virtual ~IGameSpyPatchInfo( ) { }

		virtual bool RequestPatchInfo( ) = 0;
		virtual bool HaltRequest( ) = 0;
		virtual EBrowserStatus GetPatchInfoStatus( ) = 0;
		virtual bool GetPatchInfoResults( PatchInfoResults& patchInfoResults ) = 0;

	private:

		IGameSpyPatchInfo( IGameSpyPatchInfo const& );
		IGameSpyPatchInfo& operator=( IGameSpyPatchInfo const& );
};


// ----------------------------------------------------------------------- //
// The published server and its CD key checks (Object.lto)

class GAMESPY_API IGameSpyServer
{
	public:

		struct StartupInfo
		{
			StartupInfo( )
			{
				m_nGameType = 0;
				m_nPort = 0;
				m_hSocket = 0;
				m_bPublic = true;
			}
			int				m_nGameType;
			std::string		m_sIP;			// to bind, when there is no socket
			unsigned short	m_nPort;
			unsigned int	m_hSocket;		// the game's socket, shared with GameSpy
			std::string		m_sVersion;		// "gamever"
			std::string		m_sUnused;		// set by Object.lto, never read
			bool			m_bPublic;		// list the server on the master server
		};

		enum EPublishStatus
		{
			ePublishStatus_Idle,
			ePublishStatus_Published,
			ePublishStatus_Error,
		};

		enum EChallengeResponse
		{
			eChallengeResponse_Invalid,
			eChallengeResponse_Processing,
			eChallengeResponse_Accepted,
			eChallengeResponse_Rejected,
		};

		static IGameSpyServer* Create( StartupInfo const& startupInfo );
		static void Delete( IGameSpyServer* pGameSpyServer );

		IGameSpyServer( ) { }
		virtual ~IGameSpyServer( ) { }

		virtual bool Publish( ) = 0;
		virtual bool Unpublish( ) = 0;
		virtual EPublishStatus GetPublishStatus( ) const = 0;

		// The keys the server reports, and their values.
		virtual bool RegisterKey( char const* pszKeyName ) = 0;
		virtual bool GetServerProperty( char const* pszKeyName, char* pszValue, unsigned int nValueLen ) = 0;
		virtual bool SetServerProperty( char const* pszKeyName, char const* pszValue ) = 0;
		virtual void SetNumPlayers( int nNumPlayers ) = 0;
		virtual void SetNumTeams( int nNumTeams ) = 0;

		// CD key checks of the clients.
		virtual bool GetCDKeyChallenge( char* pszChallenge, unsigned int* pnChallengeLen ) = 0;
		virtual bool ProcessCDKeyChallengeResponse( unsigned int nClientId, char const* pszClientIP,
			char const* pszChallenge, char const* pszResponse ) = 0;
		virtual EChallengeResponse GetChallengeResponse( unsigned int nClientId ) = 0;
		virtual bool ReleaseCDKey( unsigned int nClientId ) = 0;

		// A packet the game's socket received that may be GameSpy's.
		virtual bool HandleNetMessage( unsigned char const* pData, unsigned int nDataLen,
			unsigned char const* pSenderAddr, unsigned short nSenderPort ) = 0;

	private:

		IGameSpyServer( IGameSpyServer const& );
		IGameSpyServer& operator=( IGameSpyServer const& );
};

#endif // __IGAMESPY_H__
