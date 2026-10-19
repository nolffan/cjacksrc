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
#include <map>

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

	typedef std::map<std::string, CServerEntry> TServerMap;
	TServerMap	m_cServers;

	// The server whose details are asked for or shown.
	std::string	m_sSelectedServer;

	// The browsers' thread hands its servers over to the screen's.
	static void OnServerInfo( IGameSpyBrowser& gameSpyBrowser, void* hGameServer, void* pUserData );
	bool ReadServerInfo( IGameSpyBrowser& gameSpyBrowser, void* hGameServer, std::string& sKey, CServerEntry& cServer );
	void AddPendingServers();

	typedef std::vector< std::pair<std::string, CServerEntry> > TPendingServers;
	TPendingServers		m_cPendingServers;
	CRITICAL_SECTION	m_csPendingServers;

	// A server's row of the list.
	void SetupServerCtrl( CServerEntry& cServer, const char* pszKey );
	// Dump the server list into the display
	void DisplayCurServerList();

	// Read the peer details
	void ShowDetails();
	void SetDetailErrorMessage(const char *szError);

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
	EState m_eCurState;

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

	// Update handling for each state
	void Update_State_UpdateDir();
	void Update_State_QueryDetails();

	void SortServers(uint32 nSortField);

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
	// The Internet answers.
	bool	m_bInternetOk;
};

#endif // _SCREEN_JOIN_H_
