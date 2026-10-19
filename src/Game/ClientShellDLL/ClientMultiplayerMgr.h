// ----------------------------------------------------------------------- //
//
// MODULE  : ClientMultiplayerMgr.h
//
// PURPOSE : Clientside multiplayer mgr - Declaration
//
// CREATED : 02/05/02
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __CLIENTMULTIPLAYERMGR_H_
#define __CLIENTMULTIPLAYERMGR_H_

#include "VarTrack.h"
#include "NetDefs.h"
#include "ProfileUtils.h"

class IServerDirectory;
class IGameSpyBrowser;


enum eDisconnectCodes
{
	eDisconnect_None = 0,
	eDisconnect_NotSameGUID,
	eDisconnect_WrongPassword,
	// cjacksrc: Contract J.A.C.K.'s (CShell.dll 0x1001ff9f..0x1001ffc7)
	eDisconnect_BadFiles,			// The server found our files changed
	eDisconnect_CDKeyRejected,		// GameSpy rejected our CD key
	eDisconnect_Banned,				// The server bans us
};


class ClientMultiplayerMgr
{
public:
	
	ClientMultiplayerMgr( );
	~ClientMultiplayerMgr( );

	// Update called once per frame.
	void		Update( );

	bool		InitMultiPlayer();
	bool		UpdateMultiPlayer();
	bool		UpdateNetClientData( );
	bool		InitSinglePlayer();

	// Disconnect on the next update
	void		ForceDisconnect() {	m_bForceDisconnect = true;}	


	// RETAIL: 0x1001f2c0. Contract J.A.C.K.'s, from the join screen: the
	// server's GameSpy addresses ("ip:port"), whether to reach it through
	// NAT negotiation and which of its addresses to try first.
	bool		SetupClient(char const* pszHostName, char const* pszPassword, bool bNatNegotiate,
					bool bUsePublicAddress, char const* pszPublicAddress, char const* pszPrivateAddress);

	// Setup the server for singleplayer.
	bool		SetupServerSinglePlayer( );

	// Setup the server to host a multiplayer game.
	bool		SetupServerHost( int nPort, bool bLanOnly );

	// Handles engine event.
	void		OnEvent(uint32 dwEventID, uint32 dwParam);

	// Handles message from engine.
	bool		OnMessage(uint8 messageID, ILTMessage_Read *pMsg);


	void		DoTaunt(uint32 nClientID,uint32 nTaunt);


	// Sets the networking service.
	bool		SetService( );

	// Accessors to setup server information.
	StartGameRequest& GetStartGameRequest( ) { return m_StartGameRequest; }
	ServerGameOptions&	GetServerGameOptions( ) { return m_ServerGameOptions; }
	NetClientData&	GetNetClientData( ) { return m_NetClientData; }

	// Start a client of a server based on previously set startgamerequest.
	bool		StartClientServer( );

	// cjacksrc: GameSpy's browsers, for the Internet and the LAN (CShell.dll
	// 0x1001e9b0); the Internet one answers a server's CD key challenge.
	bool		CreateGameSpyBrowsers( );
	IGameSpyBrowser*	GetGameSpyBrowser( ) const { return m_pGameSpyBrowser; }
	IGameSpyBrowser*	GetGameSpyLANBrowser( ) const { return m_pGameSpyLANBrowser; }


	// This client is connected to remote server.
	bool		IsConnectedToRemoteServer( ) { return ( m_StartGameRequest.m_Type == STARTGAME_CLIENTTCP ); }

	//returns true if the passed in address matches the current server address
	bool		CheckServerAddress(char const*pszTestAddress, int nPort);
	// Called when the engine wants to tell the game a disconnection code (a.k.a. hack)
	virtual void SetDisconnectCode(uint32 nCode, const char *pMsg);
	// Internal game-side support for the disconnection code
	void		ClearDisconnectCode();
	uint32		GetDisconnectCode() { return m_nDisconnectCode; }
	const char *GetDisconnectMsg() { return m_sDisconnectMsg; }

	const char *GetServerAddress() const {return m_sServerAddress;}
	const char *GetServerName() const {return m_sServerName;}

	// RETAIL: Contract J.A.C.K. has no WON server directory (its
	// ClientMultiplayerMgr has no member for one: m_sModName is at 0x166c).
	// cjacksrc: none, for NOLF2's join screen until it is rewritten.
	IServerDirectory* GetServerDir() { return NULL; }

	// Get/Set the source address of the message which is currently being processed
	void		SetCurMessageSource(const uint8 aAddr[4], uint16 nPort);
	void		GetCurMessageSource(uint8 aAddr[4], uint16 *pPort);

	LTRESULT	GetLastConnectionResult() {return m_nLastConnectionResult;}

	void		SelectTeam(uint8 nTeam, bool bPlayerSelected);
	bool		HasSelectedTeam() { return m_bHasSelectedTeam; }

	void		SetModName( const char *pszModName ) { m_sModName = pszModName; }
	const char*	GetModName( ) const { return m_sModName.c_str(); }
	
protected :

	bool		HandleMsgHandshake( ILTMessage_Read& msg );
	bool		HandleMsgMultiplayerData( ILTMessage_Read& pMsg );
	bool		HandleMsgPlayerMultiplayerInit( ILTMessage_Read& msg );
	bool		HandleMsgPlayerSingleplayerInit (ILTMessage_Read& msg);

	bool		StartServerAsSinglePlayer( );
	bool		StartServerAsHost( );
	bool		StartClient( );

	// cjacksrc: the steps of joining a server (RETAIL: 0x1001f670).
	enum EJoinState
	{
		eJoinState_None,
		eJoinState_NatNegotiating,	// waiting for GameSpy's NAT negotiation
		eJoinState_NatWait,			// letting the server's NAT settle
		eJoinState_Join,			// ready to connect
		eJoinState_Joined,
		eJoinState_Failed,
	};

	void		UpdateJoinState( );		// RETAIL: 0x1001f670
	void		UpdateNatNegotiation( );	// RETAIL: 0x1001f190
	void		UpdateNatWait( );		// RETAIL: 0x1001f260
	void		JoinServer( );			// RETAIL: 0x1001ea70

private:

	// Must be filled out before StartLocalGame, StartNetworkGameAsXXX calls.
	StartGameRequest	m_StartGameRequest;
	ServerGameOptions	m_ServerGameOptions;
	NetClientData	m_NetClientData;


	//these refer to the server we are currently connected to
	CString		m_sServerAddress;
	int			m_nServerPort;
	CString		m_sServerName;

	//used for handshaking with server
	uint32		m_nServerKey;
	uint32		m_nClientPass;

	// Disconnection code/msg storage
	uint32		m_nDisconnectCode;
	CString		m_sDisconnectMsg;

	LTRESULT	m_nLastConnectionResult;

	// Connection handling
	bool			m_bForceDisconnect;	// Set this flag to disconnect on the next update

	// Current message source
	uint8		m_aCurMessageSourceAddr[4];
	uint16		m_nCurMessageSourcePort;


	uint8	m_nTeam;
	bool	m_bHasSelectedTeam;
	
	// The name of our selected mod...
	std::string		m_sModName;

	// cjacksrc: RETAIL: 0x1688, 0x168c (CreateGameSpyBrowsers).
	IGameSpyBrowser*	m_pGameSpyBrowser;
	IGameSpyBrowser*	m_pGameSpyLANBrowser;

	// cjacksrc: RETAIL: 0x1690 to 0x16e4 (SetupClient, StartClient, UpdateJoinState).
	bool		m_bNatNegotiate;
	EJoinState	m_eJoinState;
	float		m_fNatWaitStart;
	float		m_fNatWaitTime;
	float		m_fNatWaitNow;		// a fixed time instead of the engine's, when above 0
	bool		m_bNatNegotiated;
	bool		m_bUsePublicAddress;
	std::string	m_sPublicAddress;
	std::string	m_sPrivateAddress;
	
};

extern ClientMultiplayerMgr* g_pClientMultiplayerMgr;

#endif // __CLIENTMULTIPLAYERMGR_H_

