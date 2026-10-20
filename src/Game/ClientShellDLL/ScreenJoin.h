// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenJoin.h
//
// PURPOSE : Interface screen to search for and join LAN games
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// cjacksrc: Contract J.A.C.K.'s join screen finds its servers with GameSpy
// (CShell.dll 0x100c8f40 to 0x100d06b0), on the LAN and on the Internet,
// instead of NOLF2's WON server directory.
//
// ----------------------------------------------------------------------- //


#ifndef _SCREEN_JOIN_H_
#define _SCREEN_JOIN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseScreen.h"
#include <hash_map>

class IGameSpyBrowser;

// A player of a server's details.
struct CServerPlayer
{
	std::string	m_sName;
	uint16		m_nPing;
};

// RETAIL: a server the browsers found, 0xd0 bytes, kept by its private
// address ("ip:port").
struct CServerEntry
{
	CServerEntry( );

	std::string	m_sPublicAddress;		// "ip:port"
	std::string	m_sName;
	std::string	m_sMapName;
	std::string	m_sVersion;
	std::string	m_sModName;
	uint32		m_nNumPlayers;
	uint32		m_nMaxPlayers;
	uint16		m_nPing;				// 0xffff when unknown
	bool		m_bUsePassword;
	uint8		m_nGameType;
	std::string	m_sOptions;				// the details' "options"
	uint16		m_nFragLimit;
	uint16		m_nTimeLimit;
	std::vector<CServerPlayer>	m_Players;
	bool		m_bLANBrowser;			// found by the second browser
	bool		m_bDirectConnect;
	bool		m_bUsePublicAddress;
	CLTGUIColumnCtrl*	m_pCtrl;		// its row of the server list
};

// Orders the servers' keys without regard to case.
struct CServerKeyLess
{
	CServerKeyLess( ) { }

	bool operator()( std::string const& sKey1, std::string const& sKey2 ) const
	{
		return ( _stricmp( sKey1.c_str( ), sKey2.c_str( )) < 0 );
	}
};

// Hashes the servers' keys without regard to case (RETAIL: 0x100cc620).
class CServerKeyHashCompare : public stdext::hash_compare< std::string, CServerKeyLess >
{
	public:

		size_t operator()( std::string const& sKey ) const
		{
			size_t nHash = 0;
			for( char const* pszKey = sKey.c_str( ); *pszKey; pszKey++ )
				nHash = nHash * 5 + tolower( *pszKey );
			return nHash;
		}

		bool operator()( std::string const& sKey1, std::string const& sKey2 ) const
		{
			return comp( sKey1, sKey2 );
		}
};

// The servers found, by their private address ("ip:port").
typedef stdext::hash_map< std::string, CServerEntry, CServerKeyHashCompare > TServerMap;

class CScreenJoin : public CBaseScreen
{
public:
	CScreenJoin();
	virtual ~CScreenJoin();

	// Build the screen
    LTBOOL   Build();
	void	Term();

	LTBOOL	Render(HSURFACE hDestSurf);

    void    OnFocus(LTBOOL bFocus);
	LTBOOL	HandleKeyDown(int key, int rep);

	void	CheckPassword();

	// Forget the servers found (RETAIL: 0x100cb4a0).
	void	ClearServers();

	virtual void Escape();


protected:
    uint32  OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
	uint32	HandleCallback(uint32 dwParam1, uint32 dwParam2);

	CLTGUITextCtrl*	m_pFindCtrl;
	CLTGUIListCtrl*	m_pServerListCtrl;
	CLTGUIListCtrl*	m_pPlayerListCtrl;
	CLTGUIWindow*	m_pWait;
	CLTGUIWindow*	m_pDetails;
	CLTGUITextCtrl*	m_pJoin;
	CLTGUITextCtrl*	m_pOptions;
	CLTGUITextCtrl*	m_pMission;
	CLTGUIColumnCtrl*	m_pPort;

	// RETAIL: 0x7b0, the servers by their private address.
	TServerMap	m_cServers;

	// The server whose details are asked for or shown.
	std::string	m_sSelectedServer;

	// The browsers find their servers on their own thread (RETAIL: 0x100cb500).
	static void OnServerInfo( IGameSpyBrowser& gameSpyBrowser, void* hGameServer, void* pUserData );
	// Dump the server list into the display
	void DisplayCurServerList();

	// Read the peer details
	void ShowDetails();
	void SetDetailErrorMessage(const char *szError);
	// NOLF2's: nothing calls it (RETAIL: 0x100c9fd0).
	void ReadCoopDetails(std::string& sOptions,CLTMsgRef_Read& cRead);

	bool IsCurrentGame(const char *pAddr);
	void JoinCurGame();
	bool HaltRequests();

	enum EState {
		eState_Inactive,
		eState_Startup,
		eState_UpdateDir,
		eState_QueryDetails,
		eState_DisplaySysMOTD,
		eState_DisplayGameMOTD,
		eState_Waiting,
		eState_Transition,
		eState_Error
	};

	// Called when entering a state
	bool PreState(EState eNewState);
	// Called when leaving a state
	// Returns false if that is an invalid state transition
	bool PostState(EState eNewState);
	// Change states
	bool ChangeState(EState eNewState);
	void Update();

	// Pre-state handling for each state
	bool PreState_UpdateDir();
	bool PreState_QueryDetails();
	bool PreState_Waiting();

	// Post-state handling for each state
	bool PostState_UpdateDir(EState eNewState);
	bool PostState_QueryDetails(EState eNewState);
	bool PostState_Waiting(EState eNewState);

	// Look for servers again, unless a browser is still looking
	void FindServers();

	void Update_State_Startup();

	// Update handling for each state
	void Update_State_UpdateDir();
	void Update_State_QueryDetails();

	void SortServers(uint32 nSortField);

	EState m_eCurState;

	uint8	m_nVersionFilter;
	uint8	m_nPlayersFilter;
	uint8	m_nPingFilter;
	uint8	m_nModFilter;
	uint8	m_nGameTypeFilter;

	uint32	m_nLastSort;

	std::string	m_sPassword;

	// The LAN's port, to search around.
	char	m_szPort[8];

	// Which list request comes next (Update_State_UpdateDir).
	int		m_nSearchStep;

	// Held by the screen's update and render, and by the browsers' thread
	// to add a server's row.
	CRITICAL_SECTION	m_csServers;

	// The Internet answers.
	bool	m_bInternetOk;

#ifdef CJACK_MODERN
	// MODERN.md GAME-3: the browsers' thread hands its servers to the
	// screen's render through this queue, under its own lock, instead of
	// taking the screen's lock while it holds the browser's.
	typedef std::vector< std::pair<std::string, CServerEntry> > TPendingServers;
	TPendingServers		m_cPendingServers;
	CRITICAL_SECTION	m_csPendingServers;

	void AddServer( std::string const& sKey, CServerEntry& cServer );
	void AddPendingServers();
#endif

};

#endif // _SCREEN_JOIN_H_
