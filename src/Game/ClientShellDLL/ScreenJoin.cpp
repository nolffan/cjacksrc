// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenJoin.cpp
//
// PURPOSE : Interface screen to search for and join LAN games
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// cjacksrc: Contract J.A.C.K.'s, rebuilt from CShell.dll (0x100c8f40 to
// 0x100d06b0): GameSpy's browsers search the LAN, then the Internet, for
// servers of the retail game and of the demo.
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "ScreenJoin.h"
#include "ScreenMgr.h"
#include "ScreenCommands.h"
#include "ClientRes.h"
#include "ClientMultiplayerMgr.h"
#include "msgids.h"
#include "MissionMgr.h"
#include "MissionButeMgr.h"
#include "ProfileMgr.h"
#include "ResShared.h"
#include "IGameSpy.h"

#include "GameClientShell.h"

#include <algorithm>

// cjacksrc: Contract J.A.C.K.'s strings, which NOLF2's resource header lacks.
#define IDS_CJ_GAMETYPE_FILTER_HELP	1435	// "Set filtering of servers based on game type."
#define IDS_CJ_UNKNOWN				1436	// "Unknown"
#define IDS_CJ_LAN_PORT				1439	// "LAN Network port:"
#define IDS_CJ_QUERY_INTERNET		1448	// "Internet"
#define IDS_CJ_QUERY_INTERNET_DEMO	1449	// "Internet"
#define IDS_CJ_QUERY_LAN			1450	// "LAN"
#define IDS_CJ_QUERY_LAN_DEMO		1451	// "LAN"
#define IDS_CJ_COOPERATIVE			128		// "Cooperative"
#define IDS_CJ_DEATHMATCH			129		// "Deathmatch"
#define IDS_CJ_TIMELIMIT			131		// "Time limit"
#define IDS_CJ_RUNSPEED				132		// "Run speed"
#define IDS_CJ_FRIENDLYFIRE			133		// "Friendly fire"
#define IDS_CJ_DIFFICULTY			134		// "Difficulty"
#define IDS_CJ_PLAYERDIFF			135		// "Player difficulty factor"
#define IDS_CJ_TEAMDEATHMATCH		140		// "Team Deathmatch"
#define IDS_CJ_DOOMSDAY				141		// "Doomsday"
#define IDS_CJ_ROUNDS				142		// "Rounds"
#define IDS_CJ_DEMOLITION			143		// "Demolition"

// RETAIL: a macro, so the value's expression runs twice when it is under
// the limit.
#define ClampToUint16( nValue ) ((( nValue ) < 0xffff ) ? ( nValue ) : 0xffff )

namespace
{
	int kColumnWidth_ServerName		= 225;
	int kColumnWidth_PlayerName		= 225;
	int kColumnWidth_GameType		= 100;
	int kColumnWidth_Ping			= 60;
	int kColumnWidth_Players		= 80;
	int kColumnWidth_Lock			= 25;
	int kColumnWidth_Version		= 170;

	enum eLocalCommands
	{
		CMD_SORT_NAME = CMD_CUSTOM+1,
		CMD_SORT_PING,
		CMD_SORT_PLAYER,
		CMD_SORT_LOCK,
		CMD_SORT_VERSION,
		CMD_SORT_TYPE,
		CMD_FILTER_VERSION,
		CMD_FILTER_PLAYERS,
		CMD_FILTER_PING,
		CMD_FILTER_MOD,
		CMD_FILTER_TYPE,
		CMD_DETAILS,
	};

	// RETAIL: 0x100cc170
	void JoinConfirmationCallback(LTBOOL bReturn, void *pData)
	{
		CScreenJoin *pThisScreen = (CScreenJoin*)g_pInterfaceMgr->GetScreenMgr()->GetScreenFromID(SCREEN_ID_JOIN);
		if (bReturn && pThisScreen)
		{
			pThisScreen->CheckPassword();
		}
	}

	// RETAIL: 0x100c9030
	void EditPassCallBack(LTBOOL bReturn, void *pData)
	{
		CScreenJoin *pThisScreen = (CScreenJoin *)g_pInterfaceMgr->GetScreenMgr()->GetScreenFromID(SCREEN_ID_JOIN);
		if (bReturn && pThisScreen)
		{
			pThisScreen->SendCommand(CMD_OK,(uint32)pData,CMD_EDIT_PASS);
		}
	};

	// RETAIL: 0x100c9070
	void EditPortCallBack(LTBOOL bReturn, void *pData)
	{
		CScreenJoin *pThisScreen = (CScreenJoin *)g_pInterfaceMgr->GetScreenMgr()->GetScreenFromID(SCREEN_ID_JOIN);
		if (bReturn && pThisScreen)
		{
			pThisScreen->SendCommand(CMD_OK,(uint32)pData,CMD_EDIT_PORT);
		}
	};


}

// The sorts of the server list (RETAIL: 0x100cc5a0 to 0x100d06b0).
class CServerSortName
{
public:
	bool operator()(TServerMap::iterator x, TServerMap::iterator y) const
	{
		const CServerEntry& cX = x->second;
		const CServerEntry& cY = y->second;
		return (_stricmp(cX.m_sName.c_str(), cY.m_sName.c_str()) < 0 );
	}
};
class CServerSortVersion
{
public:
	bool operator()(TServerMap::iterator x, TServerMap::iterator y) const
	{
		// RETAIL: then by mod.
		const CServerEntry& cX = x->second;
		const CServerEntry& cY = y->second;
		int nResult = _stricmp(cX.m_sVersion.c_str(), cY.m_sVersion.c_str());
		if (nResult)
			return (nResult < 0);
		return (_stricmp(cX.m_sModName.c_str(), cY.m_sModName.c_str()) < 0);
	}
};
class CServerSortPing
{
public:
	bool operator()(TServerMap::iterator x, TServerMap::iterator y) const
	{
		// RETAIL: a server with no ping goes last.
		const CServerEntry& cX = x->second;
		const CServerEntry& cY = y->second;
		if (cX.m_nPing == 0xffff)
			return false;
		if (cY.m_nPing == 0xffff)
			return true;
		return (cX.m_nPing < cY.m_nPing );
	}
};
class CServerSortPlayers
{
public:
	bool operator()(TServerMap::iterator x, TServerMap::iterator y) const
	{
		return (x->second.m_nNumPlayers < y->second.m_nNumPlayers );
	}
};
class CServerSortLock
{
public:
	bool operator()(TServerMap::iterator x, TServerMap::iterator y) const
	{
		// RETAIL: the locked first; a lock never sorts after an open server.
		return (x->second.m_bUsePassword || !y->second.m_bUsePassword );
	}
};
class CServerSortType
{
public:
	bool operator()(TServerMap::iterator x, TServerMap::iterator y) const
	{
		return (x->second.m_nGameType < y->second.m_nGameType );
	}
};

CServerEntry::CServerEntry( ) :
	m_nNumPlayers		( 0 ),
	m_nMaxPlayers		( 0 ),
	m_nPing				( 0xffff ),
	m_bUsePassword		( false ),
	m_nGameType			( 0 ),
	m_nFragLimit		( 0 ),
	m_nTimeLimit		( 0 ),
	m_bLANBrowser		( false ),
	m_bDirectConnect	( true ),
	m_bUsePublicAddress	( true ),
	m_pCtrl				( NULL )
{
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// RETAIL: 0x100cb6e0
CScreenJoin::CScreenJoin() :
	m_pServerListCtrl	( NULL ),
	m_eCurState			( eState_Inactive ),
	m_nVersionFilter	( 0 ),
	m_nPlayersFilter	( 0 ),
	m_nPingFilter		( 0 ),
	m_nModFilter		( 0 ),
	m_nLastSort			( CMD_SORT_PING ),
	m_nSearchStep		( 0 )
{
	InitializeCriticalSection( &m_csServers );
	m_bInternetOk = true;
#ifdef CJACK_MODERN
	InitializeCriticalSection( &m_csPendingServers );
#endif
}

// RETAIL: 0x100cb7b0
CScreenJoin::~CScreenJoin()
{
	if( m_bInit )
		Term( );

	DeleteCriticalSection( &m_csServers );
#ifdef CJACK_MODERN
	DeleteCriticalSection( &m_csPendingServers );
#endif
}

// RETAIL: 0x100cb4a0
void CScreenJoin::ClearServers()
{
	if( m_bInit && m_pServerListCtrl )
		m_pServerListCtrl->RemoveAll( );
	m_cServers.clear( );
#ifdef CJACK_MODERN
	EnterCriticalSection( &m_csPendingServers );
	m_cPendingServers.clear( );
	LeaveCriticalSection( &m_csPendingServers );
#endif
}

// RETAIL: 0x100cb6d0
void CScreenJoin::Term()
{
	// RETAIL: no HaltRequests here.
	ClearServers( );
	CBaseScreen::Term( );
}

// Build the screen
// RETAIL: 0x100c90b0
LTBOOL CScreenJoin::Build()
{
 	// Make sure to call the base class
	if (!CBaseScreen::Build()) return LTFALSE;

	CreateTitle(IDS_TITLE_JOIN);

	//read layout info -----------------------------------------------
	uint8  headerFontSize = (uint8)g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOIN,"HeaderFontSize");
	LTRect serverRect = g_pLayoutMgr->GetScreenCustomRect(SCREEN_ID_JOIN,"ServerRect");

	int totalwidth = serverRect.right - serverRect.left;
	kColumnWidth_Version = totalwidth;

	kColumnWidth_ServerName = g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOIN,"NameWidth");
	kColumnWidth_Version -= kColumnWidth_ServerName;

	kColumnWidth_GameType = g_pLayoutMgr->GetScreenCustomInt( SCREEN_ID_JOIN, "TypeWidth" );
	kColumnWidth_Version -= kColumnWidth_GameType;

	kColumnWidth_Ping = g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOIN,"PingWidth");
	kColumnWidth_Version -= kColumnWidth_Ping;

	kColumnWidth_Players = g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOIN,"PlayerWidth");
	kColumnWidth_Version -= kColumnWidth_Players;

	kColumnWidth_Lock = g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOIN,"LockWidth");
	kColumnWidth_Version -= kColumnWidth_Lock;

	if (kColumnWidth_Version <= 0) kColumnWidth_Version = 50;


	//find servers button -----------------------------------------------
	LTIntPt pos = g_pLayoutMgr->GetScreenCustomPoint(SCREEN_ID_JOIN,"FindPos");
	m_pFindCtrl = AddTextItem(IDS_FIND_SERVERS,CMD_SEARCH,IDS_HELP_FIND_SERVERS,pos);
	m_pFindCtrl->Enable(LTFALSE);
	m_pFindCtrl->SetFont(NULL,headerFontSize);

	// server sort buttons -----------------------------------------------
	pos.x = serverRect.left;
	pos.y = serverRect.top;

	CLTGUITextCtrl* pCtrl = AddTextItem(IDS_SERVER_NAME,CMD_SORT_NAME,IDS_HELP_SERVER_NAME,pos);
	pCtrl->SetFont(NULL,headerFontSize);
	pos.x += kColumnWidth_ServerName;

	pCtrl = AddTextItem( IDS_GAME_TYPE, CMD_SORT_TYPE, IDS_HELP_SERVER_MOD, pos );
	pCtrl->SetFont( NULL, headerFontSize );
	pos.x += kColumnWidth_GameType;

	pCtrl = AddTextItem(IDS_SERVER_PING,CMD_SORT_PING,IDS_HELP_SERVER_PING,pos);
	pCtrl->SetFont(NULL,headerFontSize);
	pos.x += kColumnWidth_Ping;

	pCtrl = AddTextItem(IDS_SERVER_PLAYERS,CMD_SORT_PLAYER,IDS_HELP_SERVER_PLAYERS,pos);
	pCtrl->SetFont(NULL,headerFontSize);
	pos.x += kColumnWidth_Players;

	pCtrl = AddTextItem(IDS_SERVER_LOCKED,CMD_SORT_LOCK,0,pos,LTTRUE);
	pCtrl->SetFont(NULL,headerFontSize);
	pCtrl->Enable(LTFALSE);
	pos.x += kColumnWidth_Lock;

	pCtrl = AddTextItem(IDS_SERVER_MISSION,CMD_SORT_VERSION,IDS_HELP_SERVER_MISSION,pos);
	pCtrl->SetFont(NULL,headerFontSize);

	// server list -----------------------------------------------
	pos.x = serverRect.left;
	pos.y += headerFontSize + 4;
	uint16 height = serverRect.bottom - pos.y;
	m_pServerListCtrl = AddList(pos,height,LTTRUE,totalwidth);
	m_pServerListCtrl->SetScrollWrap(LTFALSE);
	m_pServerListCtrl->SetFrameWidth(2);
	m_pServerListCtrl->SetColors(m_NonSelectedColor,m_NonSelectedColor,m_DisabledColor);
	m_pServerListCtrl->SetIndent(2);


	// filter buttons -----------------------------------------------
	pos = g_pLayoutMgr->GetScreenCustomPoint(SCREEN_ID_JOIN,"BaseFilterPos");
	pCtrl = AddTextItem(IDS_SERVER_FILTERS,0,0,pos,LTTRUE);
	pCtrl->SetFont(NULL,headerFontSize);
	int kFilterNameWidth = g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOIN,"FilterNameWidth");

	// Temporarily set the item spacing for the filter controls...

	int nOldSpacing = GetItemSpacing();
	int nFilterSpacing = g_pLayoutMgr->GetScreenCustomInt( SCREEN_ID_JOIN, "FilterItemSpacing" );
	SetItemSpacing( nFilterSpacing );

	// The LAN's port, to search around.
	m_pPort = AddColumnCtrl( CMD_EDIT_PORT, IDS_HELP_ENTER_PORT );
	int nPortWidth = g_pLayoutMgr->GetScreenCustomInt( SCREEN_ID_JOIN, "PortColumnWidth" );
	m_pPort->AddColumn( LoadTempString( IDS_CJ_LAN_PORT ), nPortWidth );
	strncpy( m_szPort, "27888", sizeof( m_szPort ) - 1 );
	m_szPort[sizeof( m_szPort ) - 1] = '\0';
	m_pPort->AddColumn( m_szPort, nPortWidth, LTTRUE );

	CLTGUICycleCtrl* pCycle = AddCycle( IDS_VERSION_FILTER,IDS_HELP_VERSION_FILTER,kFilterNameWidth, &m_nVersionFilter);
	pCycle->NotifyOnChange(CMD_FILTER_VERSION,this);
	pCycle->AddString(LoadTempString(IDS_ALL_VERSIONS));
	pCycle->AddString(LoadTempString(IDS_CURRENT_VERSION));

	pCycle = AddCycle(IDS_POP_FILTER,IDS_HELP_POP_FILTER,kFilterNameWidth,&m_nPlayersFilter);
	pCycle->NotifyOnChange(CMD_FILTER_PLAYERS,this);
	pCycle->AddString(LoadTempString(IDS_FILTER_ALLPOP));
	pCycle->AddString(LoadTempString(IDS_FILTER_NOTEMPTY));
	pCycle->AddString(LoadTempString(IDS_FILTER_NOTFULL));
	pCycle->AddString(LoadTempString(IDS_FILTER_NOT_BOTH));

	pCycle = AddCycle(IDS_FILTER_PING,IDS_HELP_FILTER_PING,kFilterNameWidth,&m_nPingFilter);
	pCycle->NotifyOnChange(CMD_FILTER_PING,this);
	pCycle->AddString(LoadTempString(IDS_FILTER_ALLPING));
	pCycle->AddString(LoadTempString(IDS_FILTER_LOWPING));
	pCycle->AddString(LoadTempString(IDS_FILTER_MEDPING));
	pCycle->AddString(LoadTempString(IDS_FILTER_HIGHPING));

	// Reset the item spacing before the last filter control is added...

	SetItemSpacing( nOldSpacing );

	pCycle = AddCycle( IDS_SERVER_MOD, IDS_CJ_GAMETYPE_FILTER_HELP, kFilterNameWidth, &m_nGameTypeFilter );
	pCycle->NotifyOnChange( CMD_FILTER_TYPE, this );
	pCycle->AddString( LoadTempString( IDS_ALL ));
	pCycle->AddString( LoadTempString( IDS_CJ_DEATHMATCH ));
	pCycle->AddString( LoadTempString( IDS_CJ_TEAMDEATHMATCH ));
	pCycle->AddString( LoadTempString( IDS_CJ_DOOMSDAY ));
	pCycle->AddString( LoadTempString( IDS_CJ_DEMOLITION ));


	// "please wait" popup  window -----------------------------------------------
	CUIFont* pFont = g_pInterfaceResMgr->GetFont(g_pLayoutMgr->GetHelpFont());
	uint8 nFontSize = g_pLayoutMgr->GetHelpSize();

	CLTGUITextCtrl*	pText = debug_new(CLTGUITextCtrl);
    if (!pText->Create(FormatTempString(IDS_QUERYING, ""), NULL, LTNULL, pFont, nFontSize, this))
	{
		debug_delete(pText);
        return LTFALSE;
	}
	pText->SetColors(argbBlack, argbBlack, argbBlack);
	pText->Enable(LTFALSE);

	uint16 w = 16+pText->GetBaseWidth();
	uint16 h = 16+pText->GetBaseHeight();


	m_pWait = debug_new(CLTGUIWindow);
	char szBack[128] = "";
	g_pLayoutMgr->GetDialogFrame(szBack,sizeof(szBack));
	m_pWait->Create(g_pInterfaceResMgr->GetTexture(szBack),w,h);
	uint16 x = (640-w)/2;
	uint16 y = (480-h)/2;
	m_pWait->SetBasePos(LTIntPt(x,y));
	AddControl(m_pWait);

	m_pWait->AddControl(pText,LTIntPt(8,8));



	// details popup  window -----------------------------------------------
	LTRect detailsRect = g_pLayoutMgr->GetScreenCustomRect(SCREEN_ID_JOIN,"DetailsRect");
	uint8  optionFontSize = (uint8)g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOIN,"OptionsFontSize");
	uint16  optionHeight = (uint16)g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOIN,"OptionsHeight");
	pos.x = detailsRect.left;
	pos.y = detailsRect.top;
	uint16 nDetailsWidth = detailsRect.right-detailsRect.left;
	uint16 nDetailsHeight = detailsRect.bottom-detailsRect.top;

	m_pDetails = debug_new(CLTGUIWindow);
	g_pLayoutMgr->GetScreenCustomString(SCREEN_ID_JOIN,"DetailsFrame",szBack,sizeof(szBack));
	m_pDetails->Create(g_pInterfaceResMgr->GetTexture(szBack),nDetailsWidth,nDetailsHeight);
	m_pDetails->SetBasePos(pos);
	AddControl(m_pDetails);

	pos.x = 8;
	pos.y = 8;


	CLTGUITextCtrl* pOption = CreateTextItem(IDS_HOST_OPTIONS,0,0,pos,LTTRUE);
	pOption->SetFont(NULL,headerFontSize);
	m_pDetails->AddControl(pOption,pos);
	pos.y += (headerFontSize + 4);

	uint16 width = nDetailsWidth - 32;
	kColumnWidth_PlayerName = width - kColumnWidth_Ping;

	m_pOptions = CreateTextItem(" ",0,0,pos,LTTRUE);
	m_pOptions->SetFixedWidth(width);
	m_pOptions->SetFont(NULL,optionFontSize);
	m_pDetails->AddControl(m_pOptions,pos);
	pos.y += (optionHeight + 4);

	// The map, or its mission.
	CLTGUITextCtrl* pMap = CreateTextItem(IDS_SERVER_MAP,0,0,pos,LTTRUE);
	pMap->SetFont(NULL,optionFontSize);
	m_pDetails->AddControl(pMap,pos);

	LTIntPt missionPos( pos.x + pMap->GetBaseWidth() + 16, pos.y );
	m_pMission = CreateTextItem(IDS_SERVER_MISSION,0,0,missionPos,LTTRUE);
	m_pMission->SetFont(NULL,optionFontSize);
	m_pDetails->AddControl(m_pMission,missionPos);
	pos.y += (optionFontSize + 4);

	CLTGUITextCtrl* pPlayer = CreateTextItem(IDS_SERVER_PLAYERS,0,0,pos,LTTRUE);
	pPlayer->SetFont(NULL,headerFontSize);
	m_pDetails->AddControl(pPlayer,pos);
	pos.x += kColumnWidth_PlayerName;

	CLTGUITextCtrl* pPing = CreateTextItem(IDS_SERVER_PING,0,0,pos,LTTRUE);
	pPing->SetFont(NULL,headerFontSize);
	m_pDetails->AddControl(pPing,pos);

	pos.x = 8;
	pos.y += (headerFontSize + 4);
	height = nDetailsHeight - 2*m_nItemSpacing - headerFontSize - pos.y;

	m_pPlayerListCtrl = CreateList(pos,height,LTTRUE,width);
	m_pPlayerListCtrl->SetScrollWrap(LTFALSE);
	m_pPlayerListCtrl->SetScrollByPage(LTTRUE);
	m_pPlayerListCtrl->SetFrameWidth(1);
	m_pPlayerListCtrl->SetColors(m_NonSelectedColor,m_NonSelectedColor,m_NonSelectedColor);
	m_pPlayerListCtrl->SetIndent(2);
	m_pPlayerListCtrl->Enable(LTTRUE);
	m_pDetails->AddControl(m_pPlayerListCtrl,pos);

	CLTGUITextCtrl* pCancel = CreateTextItem(IDS_CANCEL,CMD_CANCEL,0);
	pCancel->SetFont(NULL,headerFontSize);
	pos.y += (height + m_nItemSpacing);
	m_pDetails->AddControl(pCancel,pos);

	m_pJoin = CreateTextItem(IDS_JOIN_GAME,CMD_JOIN,IDS_HELP_JOIN_GAME);
	m_pJoin->SetFont(NULL,headerFontSize);
	pos.x = (nDetailsWidth - 8) - m_pJoin->GetBaseWidth();

	m_pDetails->AddControl(m_pJoin,pos);

	m_pDetails->Show(LTFALSE);

	return LTTRUE;
}

// RETAIL: 0x100cc290
uint32 CScreenJoin::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_SORT_NAME:
	case CMD_SORT_PING:
	case CMD_SORT_PLAYER:
	case CMD_SORT_LOCK:
	case CMD_SORT_VERSION:
	case CMD_SORT_TYPE:
	{
		SortServers(dwCommand);
		break;
	}

	case CMD_FILTER_VERSION:
	case CMD_FILTER_PLAYERS:
	case CMD_FILTER_PING:
	case CMD_FILTER_MOD:
	case CMD_FILTER_TYPE:
	{
		UpdateData();
		DisplayCurServerList();
		break;
	}

	case CMD_CANCEL :
	{
		m_pDetails->Show(LTFALSE);
		SetCapture(NULL);
		ChangeState(eState_Waiting);
		break;
	}

	case CMD_DETAILS :
	{
		const char* pszKey = (const char*)dwParam1;
		if (!pszKey || !pszKey[0])
			return 0;

		m_sSelectedServer = pszKey;
		ChangeState(eState_QueryDetails);
		break;
	}

	case CMD_SEARCH :
	{
		FindServers();
		break;
	}

	case CMD_OK:
	{
		HandleCallback(dwParam1,dwParam2);
		break;
	}

	case CMD_JOIN:
	{
		m_pDetails->Show(LTFALSE);
		SetCapture(LTNULL);

		TServerMap::iterator iServer = m_cServers.find(m_sSelectedServer);
		if (iServer == m_cServers.end())
			return 0;

		CServerEntry& cServer = iServer->second;

		if (stricmp(cServer.m_sVersion.c_str(), g_pVersionMgr->GetVersion()) != 0)
		{
			MBCreate mb;
			g_pInterfaceMgr->ShowMessageBox(IDS_SERVER_WRONGVERSION,&mb);
			return 1;
		}

		if (cServer.m_nNumPlayers >= cServer.m_nMaxPlayers)
		{
			MBCreate mb;
			g_pInterfaceMgr->ShowMessageBox(IDS_SERVERFULL,&mb);
			return 1;
		}

		if( stricmp( cServer.m_sModName.c_str(), g_pClientMultiplayerMgr->GetModName() ) != 0 )
		{
			MBCreate mb;
			g_pInterfaceMgr->ShowMessageBox( IDS_SERVER_WRONGMOD, &mb );
			return 1;
		}

		CheckPassword();
		break;
	}

	case CMD_EDIT_PORT:
	{
		MBCreate mb;
		mb.eType = LTMB_EDIT;
		mb.pFn = EditPortCallBack;
		mb.pString = m_szPort;
		mb.nMaxChars = 5;
		mb.eInput = CLTGUIEditCtrl::kInputNumberOnly;
		g_pInterfaceMgr->ShowMessageBox(IDS_CJ_LAN_PORT,&mb);
		break;
	}
	default:
		return CBaseScreen::OnCommand(dwCommand,dwParam1,dwParam2);
	}
	return 1;
}

// RETAIL: 0x100cc1a0
uint32 CScreenJoin::HandleCallback(uint32 dwParam1, uint32 dwParam2)
{
	switch (dwParam2)
	{
		case CMD_EDIT_PASS :
		{
			m_sPassword = (char *)dwParam1;
			JoinCurGame();
			break;
		}
		case CMD_EDIT_PORT :
		{
			const char* pszPort = (const char*)dwParam1;
			if ((uint16)atoi(pszPort) > 0)
			{
				strncpy(m_szPort, pszPort, sizeof(m_szPort) - 1);
				m_szPort[sizeof(m_szPort) - 1] = '\0';
				m_pPort->SetString(1, m_szPort);
			}
			break;
		}
	}
	ChangeState(eState_Waiting);
	return 1;
}


// Change in focus
// RETAIL: 0x100cbdc0
void    CScreenJoin::OnFocus(LTBOOL bFocus)
{
	CUserProfile* pProfile = g_pProfileMgr->GetCurrentProfile();

	if (bFocus)
	{
		m_pWait->Show(LTTRUE);
		SetCapture(m_pWait);

		sprintf(m_szPort, "%d", pProfile->m_nClientPort);
		m_pPort->GetColumn(1)->SetString(m_szPort);

		// Search when the browsers are new: they are made again after a join.
		bool bNewBrowsers = false;
		if (!g_pClientMultiplayerMgr->GetGameSpyBrowser() || !g_pClientMultiplayerMgr->GetGameSpyLANBrowser())
		{
			g_pClientMultiplayerMgr->CreateGameSpyBrowsers();
			bNewBrowsers = true;
		}

		m_bInternetOk = true;
		if (bNewBrowsers)
			ChangeState(eState_Startup);
		else
			ChangeState(eState_Waiting);

		// we may have gotten here after failing a join, so rebuild our history
		if (m_pScreenMgr->GetLastScreenID() == SCREEN_ID_NONE)
		{
			m_pScreenMgr->AddScreenToHistory(SCREEN_ID_MAIN);
			m_pScreenMgr->AddScreenToHistory(SCREEN_ID_MULTI);
		}

		SetSelection(GetIndex(m_pFindCtrl));

        UpdateData(LTFALSE);
	}
	else
	{
		ChangeState(eState_Inactive);

		pProfile->m_nClientPort = (uint16)atoi(m_szPort);
		pProfile->Save();

		UpdateData();
	}

	CBaseScreen::OnFocus(bFocus);
}

// RETAIL: 0x100c9fd0. NOLF2's cooperative details from the old server
// directory's message, without the skills; nothing calls it.
void CScreenJoin::ReadCoopDetails(std::string& sOptions,CLTMsgRef_Read& cRead)
{
	char aStringBuffer[256];

	bool bFriendlyFire = cRead->Readbool();
	sprintf(aStringBuffer,"%s: ",LoadTempString(IDS_CJ_FRIENDLYFIRE));
	sOptions += aStringBuffer;
	if (bFriendlyFire)
		sprintf(aStringBuffer,"%s; ",LoadTempString(IDS_YES));
	else
		sprintf(aStringBuffer,"%s; ",LoadTempString(IDS_NO));
	sOptions += aStringBuffer;

	uint8 nDiff = cRead->Readuint8();
	sprintf(aStringBuffer,"%s: ",LoadTempString(IDS_CJ_DIFFICULTY));
	sOptions += aStringBuffer;
	switch  (nDiff)
	{
	case 3:
		sprintf(aStringBuffer,"%s; ",LoadTempString(IDS_NEW_INSANE));
		break;
	case 2:
		sprintf(aStringBuffer,"%s; ",LoadTempString(IDS_NEW_HARD));
		break;
	case 1:
		sprintf(aStringBuffer,"%s; ",LoadTempString(IDS_NEW_MEDIUM));
		break;
	case 0:
	default:
		sprintf(aStringBuffer,"%s; ",LoadTempString(IDS_NEW_EASY));
		break;
	}
	sOptions += aStringBuffer;

	float fPlayerDiff = cRead->Readfloat();
	sprintf(aStringBuffer,"%s: %0.2f; ",LoadTempString(IDS_CJ_PLAYERDIFF),fPlayerDiff);
	sOptions += aStringBuffer;

	//read player list
	bool bPlayerToRead = cRead->Readbool();
	while (bPlayerToRead)
	{
		cRead->ReadString(aStringBuffer, sizeof(aStringBuffer));
		uint16 nPing = cRead->Readuint16();
		bPlayerToRead = cRead->Readbool();

		// Create a control
		CLTGUIColumnCtrl* pCtrl = CreateColumnCtrl(0,0,kDefaultPos,LTTRUE);
		// Do the name
		pCtrl->AddColumn(aStringBuffer, kColumnWidth_PlayerName,LTTRUE);

		// Do the ping
		sprintf(aStringBuffer, "%d", nPing);
		pCtrl->AddColumn(aStringBuffer, kColumnWidth_Ping);

		m_pPlayerListCtrl->AddControl(pCtrl);
	}
}

// RETAIL: 0x100ca210. A server's row of the list.
static bool SetupServerCtrl( CLTGUIColumnCtrl* pCtrl, CServerEntry& cServer, const char* pszKey )
{
	if( pCtrl->GetNumColumns( ) == 0 )
	{
		pCtrl->AddColumn( "", kColumnWidth_ServerName, LTTRUE );
		pCtrl->AddColumn( "", kColumnWidth_GameType, LTTRUE );
		pCtrl->AddColumn( "", kColumnWidth_Ping, LTTRUE );
		pCtrl->AddColumn( "", kColumnWidth_Players, LTTRUE );
		pCtrl->AddColumn( "", kColumnWidth_Lock, LTTRUE );
		pCtrl->AddColumn( "", kColumnWidth_Version, LTTRUE );
	}

	char szTemp[256];

	pCtrl->GetColumn( 0 )->SetString( cServer.m_sName.c_str( ));

	switch( cServer.m_nGameType )
	{
		case eGameTypeCooperative:
			sprintf( szTemp, "%s", LoadTempString( IDS_CJ_COOPERATIVE ));
			break;
		case eGameTypeDeathmatch:
			sprintf( szTemp, "%s", LoadTempString( IDS_CJ_DEATHMATCH ));
			break;
		case eGameTypeTeamDeathmatch:
			sprintf( szTemp, "%s", LoadTempString( IDS_CJ_TEAMDEATHMATCH ));
			break;
		case eGameTypeDoomsDay:
			sprintf( szTemp, "%s", LoadTempString( IDS_CJ_DOOMSDAY ));
			break;
		case eGameTypeDemolition:
			sprintf( szTemp, "%s", LoadTempString( IDS_CJ_DEMOLITION ));
			break;
		default:
			sprintf( szTemp, "%s", LoadTempString( IDS_CJ_UNKNOWN ));
			break;
	}
	pCtrl->GetColumn( 1 )->SetString( szTemp );

	if( cServer.m_nPing == 0xffff )
		szTemp[0] = '\0';
	else
		sprintf( szTemp, "%d", cServer.m_nPing );
	pCtrl->GetColumn( 2 )->SetString( szTemp );

	sprintf( szTemp, "%d/%d", cServer.m_nNumPlayers, cServer.m_nMaxPlayers );
	pCtrl->GetColumn( 3 )->SetString( szTemp );

	if( cServer.m_bUsePassword )
		pCtrl->GetColumn( 4 )->SetString( "x" );
	else
		pCtrl->GetColumn( 4 )->SetString( " " );

	if( _stricmp( cServer.m_sModName.c_str( ), "Retail" ) == 0 )
		sprintf( szTemp, "%s", cServer.m_sVersion.c_str( ));
	else
		sprintf( szTemp, "%s, %s", cServer.m_sVersion.c_str( ), cServer.m_sModName.c_str( ));
	pCtrl->GetColumn( 5 )->SetString( szTemp );

	// Only our version, with our mod, can be joined.
	bool bEnable = true;
	if( _stricmp( cServer.m_sVersion.c_str( ), g_pVersionMgr->GetVersion( )) != 0 ||
		_stricmp( cServer.m_sModName.c_str( ), g_pClientMultiplayerMgr->GetModName( )) != 0 )
		bEnable = false;
	pCtrl->Enable( bEnable );

	pCtrl->SetParam1(( uint32 )pszKey );

	return true;
}

// RETAIL: 0x100cacc0. A server's details from its browser, and its key.
static bool ReadServerInfo( IGameSpyBrowser& gameSpyBrowser, void* hGameServer, std::string& sKey,
	CServerEntry& cServer )
{
	char szPrivateIP[256];
	char szPublicIP[256];
	unsigned short nPrivatePort;
	unsigned short nPublicPort;
	char szValue[256];

	if( !gameSpyBrowser.GetServerConnectInfo( hGameServer, szPrivateIP, &nPrivatePort, szPublicIP, &nPublicPort,
		&cServer.m_bUsePublicAddress, &cServer.m_bDirectConnect ))
		return false;

	sKey = szPrivateIP;
	sKey += ":";
	sprintf( szValue, "%d", nPrivatePort );
	sKey += szValue;

	cServer.m_sPublicAddress = szPublicIP;
	cServer.m_sPublicAddress += ":";
	sprintf( szValue, "%d", nPublicPort );
	cServer.m_sPublicAddress += szValue;

	if( !gameSpyBrowser.GetServerProperty( hGameServer, "hostname", szValue, sizeof( szValue )))
		return false;
	cServer.m_sName = szValue;

	if( !gameSpyBrowser.GetServerProperty( hGameServer, "gamever", szValue, sizeof( szValue )))
		return false;
	cServer.m_sVersion = szValue;

	if( !gameSpyBrowser.GetServerProperty( hGameServer, "modname", szValue, sizeof( szValue )))
		return false;
	cServer.m_sModName = szValue;

	cServer.m_nNumPlayers = gameSpyBrowser.GetNumPlayers( hGameServer );

	if( !gameSpyBrowser.GetServerProperty( hGameServer, "maxplayers", szValue, sizeof( szValue )))
		return false;
	cServer.m_nMaxPlayers = atoi( szValue );

	if( !gameSpyBrowser.GetServerProperty( hGameServer, "password", szValue, sizeof( szValue )))
		return false;
	cServer.m_bUsePassword = ( atoi( szValue ) != 0 );

	if( !gameSpyBrowser.GetServerProperty( hGameServer, "gametype", szValue, sizeof( szValue )))
		return false;
	cServer.m_nGameType = ( uint8 )GameStringTypeToGameType( szValue );

	cServer.m_nPing = gameSpyBrowser.GetServerPing( hGameServer );

	if( !gameSpyBrowser.HasServerDetails( hGameServer ))
		return true;

	// The details...

	if( gameSpyBrowser.GetServerProperty( hGameServer, "mapname", szValue, sizeof( szValue )))
		cServer.m_sMapName = szValue;

	if( gameSpyBrowser.GetServerProperty( hGameServer, "options", szValue, sizeof( szValue )))
		cServer.m_sOptions = szValue;

	if( gameSpyBrowser.GetServerProperty( hGameServer, "fraglimit", szValue, sizeof( szValue )))
		cServer.m_nFragLimit = ( uint16 )ClampToUint16( atoi( szValue ));

	if( gameSpyBrowser.GetServerProperty( hGameServer, "timelimit", szValue, sizeof( szValue )))
		cServer.m_nTimeLimit = ( uint16 )ClampToUint16( atoi( szValue ));

	uint32 nNumPlayers = gameSpyBrowser.GetNumPlayers( hGameServer );
	cServer.m_Players.resize( 0 );	// RETAIL: 0x100cafc8
	cServer.m_Players.reserve( nNumPlayers );
	CServerPlayer player;
	for( uint32 nPlayer = 0; nPlayer < nNumPlayers; nPlayer++ )
	{
		char szKey[256];
		sprintf( szKey, "player_%d", nPlayer );
		if( !gameSpyBrowser.GetServerProperty( hGameServer, szKey, szValue, sizeof( szValue )))
			break;

		player.m_sName = szValue;

		sprintf( szKey, "ping_%d", nPlayer );
		if( gameSpyBrowser.GetServerProperty( hGameServer, szKey, szValue, sizeof( szValue )) &&
			atoi( szValue ) < 0xffff )
		{
			player.m_nPing = ( uint16 )atoi( szValue );
		}
		else
		{
			player.m_nPing = 0xffff;
		}

		cServer.m_Players.push_back( player );
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenJoin::OnServerInfo
//
//	PURPOSE:	A browser found a server, or its details.
//
// ----------------------------------------------------------------------- //

// RETAIL: 0x100cb500. On the browser's thread: the server goes into the
// list, and a new one's row into the list control.
void CScreenJoin::OnServerInfo( IGameSpyBrowser& gameSpyBrowser, void* hGameServer, void* pUserData )
{
	CServerEntry cServer;
	std::string sKey;
	if( !ReadServerInfo( gameSpyBrowser, hGameServer, sKey, cServer ))
		return;

	CScreenJoin* pScreen = ( CScreenJoin* )pUserData;
	cServer.m_bLANBrowser = ( &gameSpyBrowser == g_pClientMultiplayerMgr->GetGameSpyLANBrowser( ));

	// The details asked for are the selected server's.
	bool bNew = false;
	if( pScreen->m_sSelectedServer.length( ))
		sKey = pScreen->m_sSelectedServer;

#ifdef CJACK_MODERN
	// RETAIL_BUGS.md: retail takes the screen's lock here, on the browser's
	// thread and holding the browser's locks, while the screen's render
	// holds the screen's lock and asks the browsers for their status.
	EnterCriticalSection( &pScreen->m_csPendingServers );
	pScreen->m_cPendingServers.push_back( std::make_pair( sKey, cServer ));
	LeaveCriticalSection( &pScreen->m_csPendingServers );
	return;
#endif

	TServerMap::iterator iServer = pScreen->m_cServers.find( sKey );
	if( iServer == pScreen->m_cServers.end( ))
	{
		bNew = true;
		cServer.m_pCtrl = pScreen->CreateColumnCtrl( CMD_DETAILS, IDS_HELP_JOIN );
		std::pair<TServerMap::iterator, bool> cInserted = pScreen->m_cServers.insert( std::pair<std::string, CServerEntry>( sKey, cServer ));
		iServer = cInserted.first;
	}
	else
	{
		cServer.m_pCtrl = iServer->second.m_pCtrl;
		iServer->second = cServer;
	}

	SetupServerCtrl( cServer.m_pCtrl, cServer, iServer->first.c_str( ));

	if( bNew )
	{
		EnterCriticalSection( &pScreen->m_csServers );
		pScreen->m_pServerListCtrl->AddControl( cServer.m_pCtrl );
		LeaveCriticalSection( &pScreen->m_csServers );
	}
}

#ifdef CJACK_MODERN
// MODERN.md GAME-3: OnServerInfo's work on the screen's thread.
void CScreenJoin::AddServer( std::string const& sKey, CServerEntry& cServer )
{
	bool bNew = false;
	TServerMap::iterator iServer = m_cServers.find( sKey );
	if( iServer == m_cServers.end( ))
	{
		bNew = true;
		cServer.m_pCtrl = CreateColumnCtrl( CMD_DETAILS, IDS_HELP_JOIN );
		iServer = m_cServers.insert( std::make_pair( sKey, cServer )).first;
	}
	else
	{
		cServer.m_pCtrl = iServer->second.m_pCtrl;
		iServer->second = cServer;
	}

	SetupServerCtrl( cServer.m_pCtrl, cServer, iServer->first.c_str( ));

	if( bNew )
		m_pServerListCtrl->AddControl( cServer.m_pCtrl );
}

void CScreenJoin::AddPendingServers()
{
	TPendingServers cPending;
	EnterCriticalSection( &m_csPendingServers );
	cPending.swap( m_cPendingServers );
	LeaveCriticalSection( &m_csPendingServers );

	TPendingServers::iterator iPending;
	for( iPending = cPending.begin( ); iPending != cPending.end( ); iPending++ )
		AddServer( iPending->first, iPending->second );
}
#endif

// RETAIL: 0x100caa60. Show the servers the filters let through.
void CScreenJoin::DisplayCurServerList()
{
	uint16 nFirst = CLTGUIListCtrl::kNoSelection;

	for( uint16 i = 0; i < m_pServerListCtrl->GetNumControls( ); i++ )
	{
		const char* pszKey = ( const char* )m_pServerListCtrl->GetControl( i )->GetParam1( );
		if( !pszKey || !pszKey[0] )
			return;

		TServerMap::iterator iServer = m_cServers.find( pszKey );
		if( iServer == m_cServers.end( ))
			return;

		CServerEntry& cServer = iServer->second;
		bool bShow = true;

		if( m_nVersionFilter )
			bShow &= ( _stricmp( cServer.m_sVersion.c_str( ), g_pVersionMgr->GetVersion( )) == 0 );

		int nGameType = cServer.m_nGameType;
		switch( m_nGameTypeFilter )
		{
			case 1: bShow &= ( nGameType == eGameTypeDeathmatch ); break;
			case 2: bShow &= ( nGameType == eGameTypeTeamDeathmatch ); break;
			case 3: bShow &= ( nGameType == eGameTypeDoomsDay ); break;
			case 4: bShow &= ( nGameType == eGameTypeDemolition ); break;
			default: break;
		}

		switch( m_nPlayersFilter )
		{
			case 1:
				bShow &= ( cServer.m_nNumPlayers > 0 );
				break;
			case 2:
				bShow &= ( cServer.m_nNumPlayers < cServer.m_nMaxPlayers );
				break;
			case 3:
				bShow &= ( cServer.m_nNumPlayers < cServer.m_nMaxPlayers ) & ( cServer.m_nNumPlayers > 0 );
				break;
			default:
				break;
		}

		switch( m_nPingFilter )
		{
			case 1: bShow &= ( cServer.m_nPing < 100 ); break;
			case 2: bShow &= ( cServer.m_nPing < 300 ); break;
			case 3: bShow &= ( cServer.m_nPing < 500 ); break;
			default: break;
		}

		switch( m_nModFilter )
		{
			case 1:
				bShow &= ( _stricmp( cServer.m_sModName.c_str( ), "Retail" ) == 0 );
				break;
			case 2:
				bShow &= ( _stricmp( cServer.m_sModName.c_str( ), g_pClientMultiplayerMgr->GetModName( )) == 0 );
				break;
			default:
				break;
		}

		if( bShow && nFirst == CLTGUIListCtrl::kNoSelection )
			nFirst = i;

		m_pServerListCtrl->GetControl( i )->Show( bShow );
	}

	if( nFirst != CLTGUIListCtrl::kNoSelection )
		m_pServerListCtrl->SetStartIndex( nFirst );

	m_pServerListCtrl->SetSelection( nFirst );
}

// RETAIL: 0x100ca530. The selected server's details.
void CScreenJoin::ShowDetails()
{
	m_pPlayerListCtrl->RemoveAll();

	TServerMap::iterator iServer = m_cServers.find( m_sSelectedServer );
	if( iServer == m_cServers.end( ))
		return;

	CServerEntry& cServer = iServer->second;

	// The mission, if the map is one of ours.
	std::string sMission( "" );
	int nMission, nLevel;
	if( g_pMissionButeMgr->IsMissionLevel( cServer.m_sMapName.c_str( ), nMission, nLevel ))
	{
		MISSION* pMission = g_pMissionButeMgr->GetMission( nMission );
		if( pMission )
		{
			if( pMission->nNameId > 0 )
				sMission = LoadTempString( pMission->nNameId );
			else if( pMission->sName.length( ))
				sMission = pMission->sName;
		}
	}

	if( !sMission.length( ))
		sMission = cServer.m_sMapName.c_str( );

	m_pMission->SetString( sMission.c_str( ));

	// The options: "FriendlyFire 1; WeaponsStay 0; Rounds 3; RunSpeed 100".
	std::string sOptions( "" );
	sOptions.reserve( 64 );

	char szOptions[256] = "";
	strncpy( szOptions, cServer.m_sOptions.c_str( ), sizeof( szOptions ) - 1 );
	szOptions[sizeof( szOptions ) - 1] = '\0';

	if( strlen( szOptions ))
	{
		char* pszToken = strtok( szOptions, ";" );
		if( pszToken )
		{
			sOptions += LoadTempString( IDS_CJ_FRIENDLYFIRE );
			sOptions += ": ";
			char const* pszValue = strpbrk( pszToken, "0123456789" );
			if( pszValue && *pszValue >= '1' )
				sOptions += LoadTempString( IDS_YES );
			else
				sOptions += LoadTempString( IDS_NO );
			sOptions += ";  ";
		}

		pszToken = strtok( NULL, ";" );
		if( pszToken )
		{
			sOptions += LoadTempString( IDS_WEAPONS_STAY );
			sOptions += ": ";
			char const* pszValue = strpbrk( pszToken, "0123456789" );
			if( pszValue && *pszValue >= '1' )
				sOptions += LoadTempString( IDS_YES );
			else
				sOptions += LoadTempString( IDS_NO );
			sOptions += ";  ";
		}

		pszToken = strtok( NULL, ";" );
		if( pszToken )
		{
			sOptions += LoadTempString( IDS_CJ_ROUNDS );
			sOptions += ": ";
			sOptions += strpbrk( pszToken, "0123456789" );
			sOptions += ";  ";
		}

		pszToken = strtok( NULL, ";" );
		if( pszToken )
		{
			sOptions += LoadTempString( IDS_CJ_RUNSPEED );
			sOptions += ": ";
			sOptions += strpbrk( pszToken, "0123456789" );
		}
	}

	char szTemp[256];
	int nGameType = cServer.m_nGameType;
	if( nGameType == eGameTypeDeathmatch || nGameType == eGameTypeTeamDeathmatch )
	{
		sprintf( szTemp, "; %s: %d", LoadTempString( IDS_SCORE_LIMIT ), cServer.m_nFragLimit );
		sOptions += szTemp;
	}

	if( nGameType != eGameTypeDemolition )
	{
		sprintf( szTemp, "; %s: %d", LoadTempString( IDS_CJ_TIMELIMIT ), cServer.m_nTimeLimit );
		sOptions += szTemp;
	}

	m_pOptions->SetString( sOptions.c_str( ));

	// The players.
	for( uint32 nPlayer = 0; nPlayer < cServer.m_Players.size( ); nPlayer++ )
	{
		CLTGUIColumnCtrl* pCtrl = CreateColumnCtrl( 0, 0, kDefaultPos, LTTRUE );
		pCtrl->AddColumn( cServer.m_Players[nPlayer].m_sName.c_str( ), kColumnWidth_PlayerName, LTTRUE );

		sprintf( szTemp, "%d", cServer.m_Players[nPlayer].m_nPing );
		pCtrl->AddColumn( szTemp, kColumnWidth_Ping, LTFALSE );

		m_pPlayerListCtrl->AddControl( pCtrl );
	}

	m_pJoin->Enable( LTTRUE );
	m_pDetails->Show( LTTRUE );
	m_pDetails->SetSelection( m_pDetails->GetIndex( m_pJoin ));
	SetCapture( m_pDetails );
}

// RETAIL: 0x100c9ec0
void CScreenJoin::SetDetailErrorMessage(const char *szError)
{
	m_pPlayerListCtrl->RemoveAll();

	if (m_pWait->IsVisible())
		m_pWait->Show(LTFALSE);
	if (m_pDetails->IsVisible())
		m_pDetails->Show(LTFALSE);
	SetCapture(NULL);

	//show edit box here
	MBCreate mb;
	g_pInterfaceMgr->ShowMessageBox(szError,&mb);

}


// RETAIL: 0x100cc230
LTBOOL CScreenJoin::Render(HSURFACE hDestSurf)
{
	EnterCriticalSection( &m_csServers );
#ifdef CJACK_MODERN
	AddPendingServers();
#endif
	Update();
	LTBOOL bResult = CBaseScreen::Render(hDestSurf);
	LeaveCriticalSection( &m_csServers );
	return bResult;
}

// RETAIL: 0x100cbee0
void CScreenJoin::FindServers()
{
	if (g_pClientMultiplayerMgr->GetGameSpyBrowser()->GetBrowserStatus() == IGameSpyBrowser::eBrowserStatus_Processing)
		return;
	if (g_pClientMultiplayerMgr->GetGameSpyLANBrowser()->GetBrowserStatus() == IGameSpyBrowser::eBrowserStatus_Processing)
		return;

	ChangeState(eState_UpdateDir);
}

// RETAIL: 0x100cbf20
LTBOOL CScreenJoin::HandleKeyDown(int key, int rep)
{
	if (key == VK_F5)
	{
		FindServers();
        return LTTRUE;
	}
    return CBaseScreen::HandleKeyDown(key,rep);
}

// RETAIL: 0x100c8f40
bool CScreenJoin::IsCurrentGame(const char *pAddr)
{
	if (!IsMultiplayerGame( )) return false;

	uint32 aAddr[4];
	uint16 nPort;	// RETAIL: "%d" writes 4 bytes into it
	aAddr[0] = 0;
	aAddr[1] = 0;
	aAddr[2] = 0;
	aAddr[3] = 0;
	sscanf(pAddr, "%d.%d.%d.%d:%d", &aAddr[0], &aAddr[1], &aAddr[2], &aAddr[3], &nPort);

	uint8 nServerAddr[4];
	uint16 nServerPort;
	g_pLTClient->GetServerIPAddress(nServerAddr, &nServerPort);

	if ((aAddr[0] == (uint32)nServerAddr[0]) &&
		(aAddr[1] == (uint32)nServerAddr[1]) &&
		(aAddr[2] == (uint32)nServerAddr[2]) &&
		(aAddr[3] == (uint32)nServerAddr[3]) &&
		(nPort == nServerPort))
		return true;
	else
		return false;

}

// RETAIL: 0x100c8ff0
bool CScreenJoin::HaltRequests()
{
	IGameSpyBrowser* pBrowser = g_pClientMultiplayerMgr->GetGameSpyBrowser();
	if (pBrowser)
		pBrowser->HaltRequest();

	IGameSpyBrowser* pLANBrowser = g_pClientMultiplayerMgr->GetGameSpyLANBrowser();
	if (pLANBrowser)
		pLANBrowser->HaltRequest();

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenJoin::JoinCurGame
//
//	PURPOSE:	Joins the selected server
//
// ----------------------------------------------------------------------- //

// RETAIL: 0x100cbf80
void CScreenJoin::JoinCurGame()
{
	TServerMap::iterator iServer = m_cServers.find( m_sSelectedServer );
	if( iServer == m_cServers.end( ))
		return;

	CServerEntry& cServer = iServer->second;

	g_pGameClientShell->SetGameType(( GameType )cServer.m_nGameType );

	if( !g_pClientMultiplayerMgr->SetupClient( cServer.m_sName.c_str( ), m_sPassword.c_str( ),
			!cServer.m_bDirectConnect, cServer.m_bUsePublicAddress,
			cServer.m_sPublicAddress.c_str( ), iServer->first.c_str( )))
	{
		ChangeState(eState_Inactive);
		g_pInterfaceMgr->LoadFailed(SCREEN_ID_JOIN);
		g_pInterfaceMgr->ConnectionFailed(g_pClientMultiplayerMgr->GetLastConnectionResult());
		return;
	}

	if( !g_pMissionMgr->StartGameAsClient( ))
	{
		ChangeState(eState_Inactive);
		g_pInterfaceMgr->LoadFailed(SCREEN_ID_JOIN);
		g_pInterfaceMgr->ConnectionFailed(g_pClientMultiplayerMgr->GetLastConnectionResult());
	}
}


// RETAIL: 0x100cb9c0
bool CScreenJoin::PreState(EState eNewState)
{
	switch (eNewState)
	{
		case eState_Inactive :
			return HaltRequests();
		case eState_UpdateDir :
			return PreState_UpdateDir();
		case eState_QueryDetails :
			return PreState_QueryDetails();
		case eState_Waiting :
			return PreState_Waiting();
		default :
			return true;
	}
}

// RETAIL: 0x100cb360
bool CScreenJoin::PostState_UpdateDir(EState eNewState)
{
	SortServers(m_nLastSort);
	return true;
}

// RETAIL: 0x100caa40
bool CScreenJoin::PostState_QueryDetails(EState eNewState)
{
	ShowDetails();
	m_pFindCtrl->Enable(LTTRUE);
	return true;
}

// RETAIL: 0x100c9f90
bool CScreenJoin::PostState_Waiting(EState eNewState)
{
	m_pFindCtrl->Enable(LTFALSE);
	m_pWait->Show(LTTRUE);
	SetCapture(m_pWait);
	return true;
}

// RETAIL: 0x100cb380
bool CScreenJoin::PostState(EState eNewState)
{
	switch (m_eCurState)
	{
		case eState_UpdateDir :
			return PostState_UpdateDir(eNewState);
		case eState_QueryDetails :
			return PostState_QueryDetails(eNewState);
		case eState_Waiting :
			return PostState_Waiting(eNewState);
		default :
			return true;
	}
}

// RETAIL: 0x100cba20
bool CScreenJoin::ChangeState(EState eNewState)
{
	// Finish the old state
	if (!PostState(m_eCurState))
		return false;

	// Enter the new state
	m_eCurState = eState_Transition;
	if (!PreState(eNewState))
	{
		// Go back to waiting
		m_pFindCtrl->Enable(LTTRUE);
		if (m_pWait->IsVisible())
			m_pWait->Show(LTFALSE);
		if (GetCapture() == m_pWait)
			SetCapture(NULL);
		m_eCurState = eState_Waiting;
		return false;
	}

	m_eCurState = eNewState;

	return true;
}

// RETAIL: 0x100cbab0
void CScreenJoin::Update_State_Startup()
{
	ChangeState(eState_UpdateDir);
}

// RETAIL: inlined in Render (0x100cc230)
void CScreenJoin::Update()
{
	switch (m_eCurState)
	{
		case eState_Startup :
			Update_State_Startup();
			break;
		case eState_UpdateDir :
			Update_State_UpdateDir();
			break;
		case eState_QueryDetails :
			Update_State_QueryDetails();
			break;
		default :
			break;
	}
}

// RETAIL: 0x100cb400
bool CScreenJoin::PreState_UpdateDir()
{
	m_pFindCtrl->Enable(LTFALSE);

	HaltRequests();

	m_pServerListCtrl->RemoveAll();
	m_cServers.clear();
#ifdef CJACK_MODERN
	EnterCriticalSection( &m_csPendingServers );
	m_cPendingServers.clear( );
	LeaveCriticalSection( &m_csPendingServers );
#endif

	m_bInternetOk = true;
	m_nSearchStep = 0;

	return true;
}

// RETAIL: 0x100cb840
bool CScreenJoin::PreState_QueryDetails()
{
	TServerMap::iterator iServer = m_cServers.find( m_sSelectedServer );
	if( iServer == m_cServers.end( ))
		return false;

	CServerEntry& cServer = iServer->second;

	if( _stricmp( cServer.m_sVersion.c_str( ), g_pVersionMgr->GetVersion( )) != 0 )
	{
		SetDetailErrorMessage( LoadTempString( IDS_SERVER_DETAIL_VERSION ));
		return false;
	}

	// The server's public "ip:port".
	const std::string& sAddress = cServer.m_sPublicAddress;
	std::string::size_type nColon = sAddress.rfind( ":", sAddress.length( ) - 1 );
	if( nColon == std::string::npos || nColon > 16 )
		return false;

	char szIP[16];
	strncpy( szIP, sAddress.c_str( ), nColon );
	szIP[nColon] = '\0';
	if( !sAddress.c_str( )[nColon + 1] )
		return false;

	unsigned short nPort = ( unsigned short )ClampToUint16( atoi( sAddress.c_str( ) + nColon + 1 ));

	IGameSpyBrowser* pBrowser = cServer.m_bLANBrowser ? g_pClientMultiplayerMgr->GetGameSpyLANBrowser( ) :
		g_pClientMultiplayerMgr->GetGameSpyBrowser( );
	if( !pBrowser->RequestServerDetails( szIP, nPort, cServer.m_bDirectConnect, OnServerInfo, this ))
	{
		SetDetailErrorMessage( LoadTempString( IDS_SERVER_NORESPONSE ));
		return false;
	}

	return true;
}

// RETAIL: 0x100c9f50
bool CScreenJoin::PreState_Waiting()
{
	m_pFindCtrl->Enable(LTTRUE);
	if (m_pWait->IsVisible())
		m_pWait->Show(LTFALSE);
	if (GetCapture() == m_pWait)
		SetCapture(NULL);
	return true;
}

// RETAIL: 0x100cbac0. Ask each browser in turn: the LAN, for the retail
// game's servers and the demo's, then the Internet, while it answers.
void CScreenJoin::Update_State_UpdateDir()
{
	IGameSpyBrowser* pWaitBrowser;		// the browser of the last request
	IGameSpyBrowser* pNextBrowser;		// the browser of the next one
	bool bInternet;						// the next request is the Internet's
	bool bCheckInternet;				// the last request was the Internet's
	int nNextStep;
	int nStatusId = -1;

	switch (m_nSearchStep)
	{
		case 0 :
			pWaitBrowser = g_pClientMultiplayerMgr->GetGameSpyBrowser();
			bCheckInternet = false;
			nNextStep = 2;
			pNextBrowser = pWaitBrowser;
			nStatusId = IDS_CJ_QUERY_LAN;
			bInternet = false;
			break;
		case 2 :
			pWaitBrowser = g_pClientMultiplayerMgr->GetGameSpyBrowser();
			pNextBrowser = g_pClientMultiplayerMgr->GetGameSpyLANBrowser();
			bCheckInternet = false;
			nNextStep = 4;
			nStatusId = IDS_CJ_QUERY_LAN_DEMO;
			bInternet = false;
			break;
		case 4 :
			pWaitBrowser = g_pClientMultiplayerMgr->GetGameSpyLANBrowser();
			pNextBrowser = g_pClientMultiplayerMgr->GetGameSpyBrowser();
			bCheckInternet = false;
			nNextStep = 1;
			bInternet = true;
			nStatusId = IDS_CJ_QUERY_INTERNET;
			break;
		case 1 :
			pWaitBrowser = g_pClientMultiplayerMgr->GetGameSpyBrowser();
			pNextBrowser = g_pClientMultiplayerMgr->GetGameSpyLANBrowser();
			bCheckInternet = true;
			nNextStep = 3;
			bInternet = true;
			nStatusId = IDS_CJ_QUERY_INTERNET_DEMO;
			break;
		case 3 :
			pWaitBrowser = g_pClientMultiplayerMgr->GetGameSpyLANBrowser();
			bCheckInternet = true;
			nNextStep = 5;
			pNextBrowser = NULL;
			bInternet = false;
			break;
		case 5 :
			// Every request is done.
			ChangeState(eState_Waiting);
			return;
		default :
			ChangeState(eState_Waiting);
			return;
	}

	// Wait for the last request.
	switch (pWaitBrowser->GetBrowserStatus())
	{
		case IGameSpyBrowser::eBrowserStatus_Idle :
		case IGameSpyBrowser::eBrowserStatus_Complete :
		case IGameSpyBrowser::eBrowserStatus_Error :
			break;
		case IGameSpyBrowser::eBrowserStatus_Processing :
			return;
		default :
			ChangeState(eState_Waiting);
			return;
	}

	if (pWaitBrowser->GetBrowserStatus() == IGameSpyBrowser::eBrowserStatus_Error && bCheckInternet)
		m_bInternetOk = false;

	pWaitBrowser->HaltRequest();

	if (pNextBrowser && (!bInternet || m_bInternetOk))
	{
		CLTGUITextCtrl* pText = (CLTGUITextCtrl*)m_pWait->GetControl(0);
		if (pText)
		{
			std::string sStatus = LoadTempString(nStatusId);
			pText->SetString(FormatTempString(IDS_QUERYING, sStatus.c_str()));

			uint16 w = 16 + pText->GetBaseWidth();
			uint16 h = 16 + pText->GetBaseHeight();
			m_pWait->SetSize(w, h);
			uint16 x = (640 - w) / 2;
			uint16 y = (480 - h) / 2;
			m_pWait->SetBasePos(LTIntPt(x, y));
		}

		unsigned short nPort = (unsigned short)atoi(m_szPort);
		m_sSelectedServer = "";
		pNextBrowser->RequestServerList(bInternet, nPort, NULL, OnServerInfo, this);
	}

	m_nSearchStep = nNextStep;
}

// RETAIL: 0x100cbd40
void CScreenJoin::Update_State_QueryDetails()
{
	if (g_pClientMultiplayerMgr->GetGameSpyBrowser()->GetBrowserStatus() == IGameSpyBrowser::eBrowserStatus_Processing)
		return;
	if (g_pClientMultiplayerMgr->GetGameSpyLANBrowser()->GetBrowserStatus() == IGameSpyBrowser::eBrowserStatus_Processing)
		return;

	// The details are in (OnServerInfo).
	ChangeState(eState_Waiting);
}

// RETAIL: 0x100cb0d0. The servers, in the sort's order.
static void GetSortedServers( std::vector<TServerMap::iterator>& cSorted, TServerMap& cServers, uint32 nSortField )
{
	cSorted.resize( 0 );
	cSorted.reserve( cServers.size( ));
	TServerMap::iterator iServer;
	for( iServer = cServers.begin( ); iServer != cServers.end( ); iServer++ )
		cSorted.push_back( iServer );

	switch( nSortField )
	{
	case CMD_SORT_NAME:
		std::sort( cSorted.begin( ), cSorted.end( ), CServerSortName( ));
		break;
	case CMD_SORT_PING:
		std::sort( cSorted.begin( ), cSorted.end( ), CServerSortPing( ));
		break;
	case CMD_SORT_PLAYER:
		std::sort( cSorted.begin( ), cSorted.end( ), CServerSortPlayers( ));
		break;
	case CMD_SORT_LOCK:
		std::sort( cSorted.begin( ), cSorted.end( ), CServerSortLock( ));
		break;
	case CMD_SORT_VERSION:
		std::sort( cSorted.begin( ), cSorted.end( ), CServerSortVersion( ));
		break;
	case CMD_SORT_TYPE:
		std::sort( cSorted.begin( ), cSorted.end( ), CServerSortType( ));
		break;
	}
}

// RETAIL: 0x100cb270
void CScreenJoin::SortServers(uint32 nSortField)
{
	m_pServerListCtrl->RemoveAll(LTFALSE);
	m_nLastSort = nSortField;

	std::vector<TServerMap::iterator> cSorted;
	GetSortedServers( cSorted, m_cServers, nSortField );

	uint16 nSelected = CLTGUIListCtrl::kNoSelection;
	std::vector<TServerMap::iterator>::iterator iSorted;
	for( iSorted = cSorted.begin( ); iSorted != cSorted.end( ); iSorted++ )
	{
		TServerMap::value_type& cServer = **iSorted;
		if( !cServer.second.m_pCtrl )
			continue;

		uint16 nIndex = m_pServerListCtrl->AddControl( cServer.second.m_pCtrl );
		if( _stricmp( cServer.first.c_str( ), m_sSelectedServer.c_str( )) == 0 )
			nSelected = nIndex;
	}

	m_pServerListCtrl->SetSelection(nSelected);
	DisplayCurServerList();
}


// RETAIL: 0x100cc0c0
void CScreenJoin::CheckPassword()
{
	TServerMap::iterator iServer = m_cServers.find( m_sSelectedServer );
	if( iServer == m_cServers.end( ))
		return;

	m_sPassword = "";
	if (iServer->second.m_bUsePassword)
	{
		//show edit box here
		MBCreate mb;
		mb.eType = LTMB_EDIT;
		mb.pFn = EditPassCallBack;
		mb.pString = "";
		mb.nMaxChars = MAX_PASSWORD-1;
		g_pInterfaceMgr->ShowMessageBox(IDS_PASSWORD,&mb);
	}
	else
	{
		JoinCurGame();
	}
}

// RETAIL: 0x100cbd80
void CScreenJoin::Escape()
{
	if (m_pDetails->IsVisible())
	{
		m_pDetails->Show(LTFALSE);
		SetCapture(NULL);
		ChangeState(eState_Waiting);
		return;
	}


	CBaseScreen::Escape();
}
