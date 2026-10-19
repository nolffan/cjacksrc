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
#define IDS_CJ_TEAMDEATHMATCH		140		// "Team Deathmatch"
#define IDS_CJ_DOOMSDAY				141		// "Doomsday"
#define IDS_CJ_ROUNDS				142		// "Rounds"
#define IDS_CJ_DEMOLITION			143		// "Demolition"

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

	int ClampToUint16( int nValue )
	{
		return ( nValue < 0xffff ) ? nValue : 0xffff;
	}

	// The game type's name, for a server's row.
	int GetGameTypeStringId( uint8 nGameType )
	{
		switch( nGameType )
		{
			case eGameTypeCooperative:		return IDS_CJ_COOPERATIVE;
			case eGameTypeDeathmatch:		return IDS_CJ_DEATHMATCH;
			case eGameTypeTeamDeathmatch:	return IDS_CJ_TEAMDEATHMATCH;
			case eGameTypeDoomsDay:			return IDS_CJ_DOOMSDAY;
			case eGameTypeDemolition:		return IDS_CJ_DEMOLITION;
			default:						return IDS_CJ_UNKNOWN;
		}
	}

	// A server's option, "Name 1", as "Label: Yes" or "Label: 1".
	void AddOption( std::string& sOptions, int nLabelId, char const* pszToken, bool bYesNo )
	{
		if( !pszToken )
			return;

		sOptions += LoadTempString( nLabelId );
		sOptions += ": ";

		char const* pszValue = strpbrk( pszToken, "0123456789" );
		if( bYesNo )
		{
			sOptions += LoadTempString(( pszValue && *pszValue >= '1' ) ? IDS_YES : IDS_NO );
		}
		else if( pszValue )
		{
			sOptions += pszValue;
		}
	}
}

// The sorts of the server list (RETAIL: 0x100cc5a0 to 0x100d06b0).
class CServerSortName
{
public:
	bool operator()(const CServerEntry* x, const CServerEntry* y) const
	{
		return (stricmp(x->m_sName.c_str(), y->m_sName.c_str()) < 0 );
	}
};
class CServerSortVersion
{
public:
	bool operator()(const CServerEntry* x, const CServerEntry* y) const
	{
		return (stricmp(x->m_sVersion.c_str(), y->m_sVersion.c_str()) < 0 );
	}
};
class CServerSortPing
{
public:
	bool operator()(const CServerEntry* x, const CServerEntry* y) const
	{
		return (x->m_nPing < y->m_nPing );
	}
};
class CServerSortPlayers
{
public:
	bool operator()(const CServerEntry* x, const CServerEntry* y) const
	{
		return (x->m_nNumPlayers < y->m_nNumPlayers );
	}
};
class CServerSortLock
{
public:
	bool operator()(const CServerEntry* x, const CServerEntry* y) const
	{
		return (x->m_bUsePassword && !y->m_bUsePassword );
	}
};
class CServerSortType
{
public:
	bool operator()(const CServerEntry* x, const CServerEntry* y) const
	{
		return (x->m_nGameType < y->m_nGameType );
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
	m_pFindCtrl			( NULL ),
	m_pServerListCtrl	( NULL ),
	m_pPlayerListCtrl	( NULL ),
	m_pWait				( NULL ),
	m_pDetails			( NULL ),
	m_pJoin				( NULL ),
	m_pOptions			( NULL ),
	m_pMission			( NULL ),
	m_pPort				( NULL ),
	m_eCurState			( eState_Inactive ),
	m_nVersionFilter	( 0 ),
	m_nPlayersFilter	( 0 ),
	m_nPingFilter		( 0 ),
	m_nModFilter		( 0 ),
	m_nGameTypeFilter	( 0 ),
	m_nLastSort			( CMD_SORT_PING ),
	m_nSearchStep		( 0 ),
	m_bInternetOk		( true )
{
	strcpy( m_szPort, "27888" );
	InitializeCriticalSection( &m_csPendingServers );
}

// RETAIL: 0x100cb7b0
CScreenJoin::~CScreenJoin()
{
	if( m_bInit )
		Term( );

	DeleteCriticalSection( &m_csPendingServers );
}

// RETAIL: 0x100cb4a0
void CScreenJoin::ClearServers()
{
	if( m_bInit && m_pServerListCtrl )
		m_pServerListCtrl->RemoveAll( );
	m_cServers.clear( );

	EnterCriticalSection( &m_csPendingServers );
	m_cPendingServers.clear( );
	LeaveCriticalSection( &m_csPendingServers );
}

// RETAIL: 0x100cb6d0
void CScreenJoin::Term()
{
	HaltRequests( );
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
		IGameSpyBrowser* pBrowser = g_pClientMultiplayerMgr->GetGameSpyBrowser();
		IGameSpyBrowser* pLANBrowser = g_pClientMultiplayerMgr->GetGameSpyLANBrowser();
		if ((!pBrowser || pBrowser->GetBrowserStatus() != IGameSpyBrowser::eBrowserStatus_Processing) &&
			(!pLANBrowser || pLANBrowser->GetBrowserStatus() != IGameSpyBrowser::eBrowserStatus_Processing))
		{
			ChangeState(eState_UpdateDir);
		}
		break;
	}

	case CMD_OK:
	{
		HandleCallback(dwParam1,dwParam2);
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
		m_pPort->SetString(1, m_szPort);

		// Search when the browsers are new: they are made again after a join.
		bool bNewBrowsers = false;
		if (!g_pClientMultiplayerMgr->GetGameSpyBrowser() || !g_pClientMultiplayerMgr->GetGameSpyLANBrowser())
		{
			g_pClientMultiplayerMgr->CreateGameSpyBrowsers();
			bNewBrowsers = true;
		}

		m_bInternetOk = true;
		ChangeState(bNewBrowsers ? eState_Startup : eState_Waiting);

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

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenJoin::OnServerInfo
//
//	PURPOSE:	A browser found a server, or its details.
//
// ----------------------------------------------------------------------- //

// RETAIL: 0x100cb500. Retail made and added the server's row on the
// browser's thread; cjacksrc reads the server there and leaves the rows to
// the screen's update, so neither thread waits on the other's lock.
void CScreenJoin::OnServerInfo( IGameSpyBrowser& gameSpyBrowser, void* hGameServer, void* pUserData )
{
	CScreenJoin* pScreen = ( CScreenJoin* )pUserData;
	if( !pScreen )
		return;

	std::pair<std::string, CServerEntry> server;
	if( !pScreen->ReadServerInfo( gameSpyBrowser, hGameServer, server.first, server.second ))
		return;

	server.second.m_bLANBrowser = ( &gameSpyBrowser == g_pClientMultiplayerMgr->GetGameSpyLANBrowser( ));


	EnterCriticalSection( &pScreen->m_csPendingServers );
	pScreen->m_cPendingServers.push_back( server );
	LeaveCriticalSection( &pScreen->m_csPendingServers );
}

// RETAIL: 0x100cacc0
bool CScreenJoin::ReadServerInfo( IGameSpyBrowser& gameSpyBrowser, void* hGameServer, std::string& sKey,
	CServerEntry& cServer )
{
	char szPrivateIP[256];
	char szPublicIP[256];
	unsigned short nPrivatePort = 0;
	unsigned short nPublicPort = 0;
	char szValue[256];

	if( !gameSpyBrowser.GetServerConnectInfo( hGameServer, szPrivateIP, &nPrivatePort, szPublicIP, &nPublicPort,
		&cServer.m_bUsePublicAddress, &cServer.m_bDirectConnect ))
		return false;

	sprintf( szValue, "%s:%d", szPrivateIP, nPrivatePort );
	sKey = szValue;
	sprintf( szValue, "%s:%d", szPublicIP, nPublicPort );
	cServer.m_sPublicAddress = szValue;

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

	int nNumPlayers = gameSpyBrowser.GetNumPlayers( hGameServer );
	cServer.m_Players.clear( );
	cServer.m_Players.reserve( nNumPlayers );
	for( int nPlayer = 0; nPlayer < nNumPlayers; nPlayer++ )
	{
		char szKey[256];
		sprintf( szKey, "player_%d", nPlayer );
		if( !gameSpyBrowser.GetServerProperty( hGameServer, szKey, szValue, sizeof( szValue )))
			break;

		CServerPlayer player;
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

// Add or update the rows of the servers the browsers found.
void CScreenJoin::AddPendingServers()
{
	TPendingServers cPending;
	EnterCriticalSection( &m_csPendingServers );
	cPending.swap( m_cPendingServers );
	LeaveCriticalSection( &m_csPendingServers );

	TPendingServers::iterator iPending;
	for( iPending = cPending.begin( ); iPending != cPending.end( ); iPending++ )
	{
		bool bNew = false;
		TServerMap::iterator iServer = m_cServers.find( iPending->first );
		if( iServer == m_cServers.end( ))
		{
			bNew = true;
			iServer = m_cServers.insert( TServerMap::value_type( iPending->first, iPending->second )).first;
			iServer->second.m_pCtrl = CreateColumnCtrl( CMD_DETAILS, IDS_HELP_JOIN );
		}
		else
		{
			CLTGUIColumnCtrl* pCtrl = iServer->second.m_pCtrl;
			iServer->second = iPending->second;
			iServer->second.m_pCtrl = pCtrl;
		}

		SetupServerCtrl( iServer->second, iServer->first.c_str( ));

		if( bNew )
			m_pServerListCtrl->AddControl( iServer->second.m_pCtrl );
	}
}

// RETAIL: 0x100ca210
void CScreenJoin::SetupServerCtrl( CServerEntry& cServer, const char* pszKey )
{
	CLTGUIColumnCtrl* pCtrl = cServer.m_pCtrl;
	if( !pCtrl )
		return;

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

	pCtrl->SetString( 0, cServer.m_sName.c_str( ));

	sprintf( szTemp, "%s", LoadTempString( GetGameTypeStringId( cServer.m_nGameType )));
	pCtrl->SetString( 1, szTemp );

	if( cServer.m_nPing == 0xffff )
		szTemp[0] = '\0';
	else
		sprintf( szTemp, "%d", cServer.m_nPing );
	pCtrl->SetString( 2, szTemp );

	sprintf( szTemp, "%d/%d", cServer.m_nNumPlayers, cServer.m_nMaxPlayers );
	pCtrl->SetString( 3, szTemp );

	pCtrl->SetString( 4, cServer.m_bUsePassword ? "x" : " " );

	if( strcmp( cServer.m_sModName.c_str( ), "Retail" ) == 0 )
		sprintf( szTemp, "%s", cServer.m_sVersion.c_str( ));
	else
		sprintf( szTemp, "%s, %s", cServer.m_sVersion.c_str( ), cServer.m_sModName.c_str( ));
	pCtrl->SetString( 5, szTemp );

	// Only our version, with our mod, can be joined.
	bool bEnable = ( strcmp( cServer.m_sVersion.c_str( ), g_pVersionMgr->GetVersion( )) == 0 &&
		strcmp( cServer.m_sModName.c_str( ), g_pClientMultiplayerMgr->GetModName( )) == 0 );
	pCtrl->Enable( bEnable );

	pCtrl->SetParam1(( uint32 )pszKey );
}

// RETAIL: 0x100caa60. Show the servers the filters let through.
void CScreenJoin::DisplayCurServerList()
{
	uint16 nFirst = CLTGUIListCtrl::kNoSelection;

	for( uint16 i = 0; i < m_pServerListCtrl->GetNumControls( ); i++ )
	{
		CLTGUICtrl* pCtrl = m_pServerListCtrl->GetControl( i );
		const char* pszKey = ( const char* )pCtrl->GetParam1( );
		if( !pszKey || !pszKey[0] )
			return;

		TServerMap::iterator iServer = m_cServers.find( pszKey );
		if( iServer == m_cServers.end( ))
			return;

		CServerEntry& cServer = iServer->second;
		bool bShow = true;

		if( m_nVersionFilter )
			bShow = ( strcmp( cServer.m_sVersion.c_str( ), g_pVersionMgr->GetVersion( )) == 0 );

		switch( m_nGameTypeFilter )
		{
			case 1: bShow &= ( cServer.m_nGameType == eGameTypeDeathmatch ); break;
			case 2: bShow &= ( cServer.m_nGameType == eGameTypeTeamDeathmatch ); break;
			case 3: bShow &= ( cServer.m_nGameType == eGameTypeDoomsDay ); break;
			case 4: bShow &= ( cServer.m_nGameType == eGameTypeDemolition ); break;
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
				bShow &= ( cServer.m_nNumPlayers > 0 && cServer.m_nNumPlayers < cServer.m_nMaxPlayers );
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
				bShow &= ( strcmp( cServer.m_sModName.c_str( ), "Retail" ) == 0 );
				break;
			case 2:
				bShow &= ( strcmp( cServer.m_sModName.c_str( ), g_pClientMultiplayerMgr->GetModName( )) == 0 );
				break;
			default:
				break;
		}

		if( bShow && nFirst == CLTGUIListCtrl::kNoSelection )
			nFirst = i;

		pCtrl->Show( bShow );
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
	std::string sMission;
	int nMission, nLevel;
	if( g_pMissionButeMgr->IsMissionLevel( cServer.m_sMapName.c_str( ), nMission, nLevel ))
	{
		MISSION* pMission = g_pMissionButeMgr->GetMission( nMission );
		if( pMission )
		{
			if( pMission->nNameId > 0 )
				sMission = LoadTempString( pMission->nNameId );
			else if( !pMission->sName.empty( ))
				sMission = pMission->sName;
		}
	}

	if( sMission.empty( ))
		sMission = cServer.m_sMapName;

	m_pMission->SetString( sMission.c_str( ));

	// The options: "FriendlyFire 1; WeaponsStay 0; Rounds 3; RunSpeed 100".
	std::string sOptions;
	sOptions.reserve( 64 );

	char szOptions[256];
	strncpy( szOptions, cServer.m_sOptions.c_str( ), sizeof( szOptions ) - 1 );
	szOptions[sizeof( szOptions ) - 1] = '\0';

	if( szOptions[0] )
	{
		char* pszToken = strtok( szOptions, ";" );
		if( pszToken )
		{
			AddOption( sOptions, IDS_CJ_FRIENDLYFIRE, pszToken, true );
			sOptions += ";  ";
		}

		pszToken = strtok( NULL, ";" );
		if( pszToken )
		{
			AddOption( sOptions, IDS_WEAPONS_STAY, pszToken, true );
			sOptions += ";  ";
		}

		pszToken = strtok( NULL, ";" );
		if( pszToken )
		{
			AddOption( sOptions, IDS_CJ_ROUNDS, pszToken, false );
			sOptions += ";  ";
		}

		pszToken = strtok( NULL, ";" );
		if( pszToken )
			AddOption( sOptions, IDS_CJ_RUNSPEED, pszToken, false );
	}

	char szTemp[256];
	if( cServer.m_nGameType == eGameTypeDeathmatch || cServer.m_nGameType == eGameTypeTeamDeathmatch )
	{
		sprintf( szTemp, "; %s: %d", LoadTempString( IDS_SCORE_LIMIT ), cServer.m_nFragLimit );
		sOptions += szTemp;
	}

	if( cServer.m_nGameType != eGameTypeDemolition )
	{
		sprintf( szTemp, "; %s: %d", LoadTempString( IDS_CJ_TIMELIMIT ), cServer.m_nTimeLimit );
		sOptions += szTemp;
	}

	m_pOptions->SetString( sOptions.c_str( ));

	// The players.
	std::vector<CServerPlayer>::iterator iPlayer;
	for( iPlayer = cServer.m_Players.begin( ); iPlayer != cServer.m_Players.end( ); iPlayer++ )
	{
		CLTGUIColumnCtrl* pCtrl = CreateColumnCtrl( 0, 0, kDefaultPos, LTTRUE );
		pCtrl->AddColumn( iPlayer->m_sName.c_str( ), kColumnWidth_PlayerName, LTTRUE );

		sprintf( szTemp, "%d", iPlayer->m_nPing );
		pCtrl->AddColumn( szTemp, kColumnWidth_Ping );

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
	Update();
	return CBaseScreen::Render(hDestSurf);
}

// RETAIL: 0x100cbf20
LTBOOL CScreenJoin::HandleKeyDown(int key, int rep)
{
	if (key == VK_F5)
	{
		IGameSpyBrowser* pBrowser = g_pClientMultiplayerMgr->GetGameSpyBrowser();
		IGameSpyBrowser* pLANBrowser = g_pClientMultiplayerMgr->GetGameSpyLANBrowser();
		if ((!pBrowser || pBrowser->GetBrowserStatus() != IGameSpyBrowser::eBrowserStatus_Processing) &&
			(!pLANBrowser || pLANBrowser->GetBrowserStatus() != IGameSpyBrowser::eBrowserStatus_Processing))
		{
			ChangeState(eState_UpdateDir);
		}
        return LTTRUE;
	}
    return CBaseScreen::HandleKeyDown(key,rep);
}

// RETAIL: 0x100c8f40
bool CScreenJoin::IsCurrentGame(const char *pAddr)
{
	if (!IsMultiplayerGame( )) return false;

	uint32 aAddr[4];
	uint32 nPort = 0;
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
		(nPort == (uint32)nServerPort))
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
			cServer.m_sPublicAddress.c_str( ), iServer->first.c_str( )) ||
		!g_pMissionMgr->StartGameAsClient( ))
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

// RETAIL: 0x100cb380
bool CScreenJoin::PostState(EState eNewState)
{
	switch (m_eCurState)
	{
		case eState_UpdateDir :
			SortServers(m_nLastSort);
			return true;
		case eState_QueryDetails :
			ShowDetails();
			m_pFindCtrl->Enable(LTTRUE);
			return true;
		case eState_Waiting :
			m_pFindCtrl->Enable(LTFALSE);
			m_pWait->Show(LTTRUE);
			SetCapture(m_pWait);
			return true;
		default :
			return true;
	}
}

// RETAIL: 0x100cba20
bool CScreenJoin::ChangeState(EState eNewState)
{
	// Finish the old state
	if (!PostState(eNewState))
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

// RETAIL: 0x100cc090
void CScreenJoin::Update()
{
	AddPendingServers();

	switch (m_eCurState)
	{
		case eState_Startup :
			ChangeState(eState_UpdateDir);
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

	EnterCriticalSection( &m_csPendingServers );
	m_cPendingServers.clear();
	LeaveCriticalSection( &m_csPendingServers );

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

	if( strcmp( cServer.m_sVersion.c_str( ), g_pVersionMgr->GetVersion( )) != 0 )
	{
		SetDetailErrorMessage( LoadTempString( IDS_SERVER_DETAIL_VERSION ));
		return false;
	}

	// "ip:port"
	const std::string& sAddress = iServer->first;
	std::string::size_type nColon = sAddress.find_last_of( ':' );
	if( nColon == std::string::npos || nColon > 16 || nColon + 1 >= sAddress.length( ))
		return false;

	char szIP[17];
	strncpy( szIP, sAddress.c_str( ), nColon );
	szIP[nColon] = '\0';
	unsigned short nPort = ( unsigned short )ClampToUint16( atoi( sAddress.c_str( ) + nColon + 1 ));

	IGameSpyBrowser* pBrowser = cServer.m_bLANBrowser ? g_pClientMultiplayerMgr->GetGameSpyLANBrowser( ) :
		g_pClientMultiplayerMgr->GetGameSpyBrowser( );
	if( !pBrowser || !pBrowser->RequestServerDetails( szIP, nPort, cServer.m_bDirectConnect, OnServerInfo, this ))
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
	IGameSpyBrowser* pBrowser = g_pClientMultiplayerMgr->GetGameSpyBrowser();
	IGameSpyBrowser* pLANBrowser = g_pClientMultiplayerMgr->GetGameSpyLANBrowser();

	IGameSpyBrowser* pWaitBrowser;		// the browser of the last request
	IGameSpyBrowser* pNextBrowser;		// the browser of the next one
	bool bInternet = false;				// the next request is the Internet's
	bool bCheckInternet = false;		// the last request was the Internet's
	int nNextStep;
	int nStatusId = 0;

	switch (m_nSearchStep)
	{
		case 0 :
			pWaitBrowser = pBrowser;
			pNextBrowser = pBrowser;
			nNextStep = 2;
			nStatusId = IDS_CJ_QUERY_LAN;
			break;
		case 2 :
			pWaitBrowser = pBrowser;
			pNextBrowser = pLANBrowser;
			nNextStep = 4;
			nStatusId = IDS_CJ_QUERY_LAN_DEMO;
			break;
		case 4 :
			pWaitBrowser = pLANBrowser;
			pNextBrowser = pBrowser;
			bInternet = true;
			nNextStep = 1;
			nStatusId = IDS_CJ_QUERY_INTERNET;
			break;
		case 1 :
			pWaitBrowser = pBrowser;
			pNextBrowser = pLANBrowser;
			bInternet = true;
			bCheckInternet = true;
			nNextStep = 3;
			nStatusId = IDS_CJ_QUERY_INTERNET_DEMO;
			break;
		case 3 :
			pWaitBrowser = pLANBrowser;
			pNextBrowser = NULL;
			bCheckInternet = true;
			nNextStep = 5;
			break;
		default :
			ChangeState(eState_Waiting);
			return;
	}

	if (!pWaitBrowser)
	{
		ChangeState(eState_Waiting);
		return;
	}

	// Wait for the last request.
	IGameSpyBrowser::EBrowserStatus eStatus = pWaitBrowser->GetBrowserStatus();
	if (eStatus == IGameSpyBrowser::eBrowserStatus_Processing)
		return;

	if (eStatus == IGameSpyBrowser::eBrowserStatus_Error && bCheckInternet)
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
			m_pWait->SetBasePos(LTIntPt((640 - w) / 2, (480 - h) / 2));
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
	IGameSpyBrowser* pBrowser = g_pClientMultiplayerMgr->GetGameSpyBrowser();
	IGameSpyBrowser* pLANBrowser = g_pClientMultiplayerMgr->GetGameSpyLANBrowser();
	if ((pBrowser && pBrowser->GetBrowserStatus() == IGameSpyBrowser::eBrowserStatus_Processing) ||
		(pLANBrowser && pLANBrowser->GetBrowserStatus() == IGameSpyBrowser::eBrowserStatus_Processing))
		return;

	// The details are in (OnServerInfo).
	AddPendingServers();
	ChangeState(eState_Waiting);
}

// RETAIL: 0x100cb270
void CScreenJoin::SortServers(uint32 nSortField)
{
	m_pServerListCtrl->RemoveAll(LTFALSE);
	m_nLastSort = nSortField;

	std::vector<CServerEntry*> cSorted;
	cSorted.reserve(m_cServers.size());
	TServerMap::iterator iServer;
	for (iServer = m_cServers.begin(); iServer != m_cServers.end(); iServer++)
		cSorted.push_back(&iServer->second);

	switch(nSortField)
	{
	case CMD_SORT_NAME:
		std::sort(cSorted.begin(),cSorted.end(),CServerSortName());
		break;
	case CMD_SORT_PING:
		std::sort(cSorted.begin(),cSorted.end(),CServerSortPing());
		break;
	case CMD_SORT_PLAYER:
		std::sort(cSorted.begin(),cSorted.end(),CServerSortPlayers());
		break;
	case CMD_SORT_LOCK:
		std::sort(cSorted.begin(),cSorted.end(),CServerSortLock());
		break;
	case CMD_SORT_VERSION:
		std::sort(cSorted.begin(),cSorted.end(),CServerSortVersion());
		break;
	case CMD_SORT_TYPE:
		std::sort(cSorted.begin(),cSorted.end(),CServerSortType());
		break;
	default:
		break;
	};

	uint16 nSelected = CLTGUIListCtrl::kNoSelection;
	std::vector<CServerEntry*>::iterator iSorted;
	for (iSorted = cSorted.begin(); iSorted != cSorted.end(); iSorted++)
	{
		CServerEntry* pServer = *iSorted;
		if (!pServer->m_pCtrl)
			continue;

		uint16 nIndex = m_pServerListCtrl->AddControl(pServer->m_pCtrl);
		const char* pszKey = (const char*)pServer->m_pCtrl->GetParam1();
		if (pszKey && m_sSelectedServer == pszKey)
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
