// ----------------------------------------------------------------------- //
//
// MODULE  : DemolitionMissionMgr.cpp
//
// PURPOSE : Contract J.A.C.K.'s Demolition game type (RETAIL: Object
//           0x100c09a0..0x100c0d50).
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "DemolitionMissionMgr.h"
#include "TeamMgr.h"
#include "MissionButeMgr.h"
#include "PlayerObj.h"
#include "ServerSaveLoadMgr.h"

CDemolitionMissionMgr::CDemolitionMissionMgr()
:	CServerMissionMgr	()
{
}

// RETAIL: Object 0x100c0d20
CDemolitionMissionMgr::~CDemolitionMissionMgr()
{
	Term( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CDemolitionMissionMgr::Init
//
//	PURPOSE:	RETAIL: Object 0x100c09d0: the host's teams
//
// ----------------------------------------------------------------------- //

bool CDemolitionMissionMgr::Init()
{
	if( !CServerMissionMgr::Init() )
		return false;

	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );

	uint8 nNumTeams = sgo.GetDemolition().m_nNumTeams;
	for( uint8 i = 0; i < nNumTeams; ++i )
	{
		CTeamMgr::Instance().AddTeam( sgo.GetDemolition().m_sTeamName[i].c_str(),
									  sgo.GetDemolition().m_nTeamModel[i] );
	}

	return !!CTeamMgr::Instance().GetNumTeams();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CDemolitionMissionMgr::Update
//
//	PURPOSE:	RETAIL: Object 0x100c0a40
//
// ----------------------------------------------------------------------- //

void CDemolitionMissionMgr::Update()
{
	if( m_ServerSettings.m_nScoreLimit > 0 && m_fStartTime >= 0.0f )
	{
		for( uint8 i = 0; i < CTeamMgr::Instance().GetNumTeams(); ++i )
		{
			CTeam *pTeam = CTeamMgr::Instance().GetTeam( i );
			if( pTeam && (pTeam->GetScore() >= m_ServerSettings.m_nScoreLimit) )
			{
				CTeamMgr::Instance( ).WonRound( i );
				g_pLTServer->CPrint( "ScoreLimit reached." );
				NextRound();
			}
		}
	}

	CServerMissionMgr::Update( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CDemolitionMissionMgr::ApplyOptions
//
//	PURPOSE:	The host's Demolition options as the mission settings
//
// ----------------------------------------------------------------------- //

void CDemolitionMissionMgr::ApplyOptions( )
{
	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );
	ServerMissionSettings sms = GetServerSettings();

	sms.m_bUseSkills = false;
	sms.m_bFriendlyFire = sgo.GetDemolition().m_bFriendlyFire;
	sms.m_bWeaponsStay = sgo.GetDemolition().m_bWeaponsStay;
	sms.m_nRunSpeed = sgo.GetDemolition().m_nRunSpeed;
	sms.m_nScoreLimit = 0;
	sms.m_nTimeLimit = 0;
	sms.m_nRounds = sgo.GetDemolition().m_nRounds;
	sms.m_nFragScore = sgo.GetDemolition().m_nFragScore;

	SetServerSettings(sms);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CDemolitionMissionMgr::StartGame
//
//	PURPOSE:	RETAIL: Object 0x100c0af0
//
// ----------------------------------------------------------------------- //

bool CDemolitionMissionMgr::StartGame(ILTMessage_Read& msg )
{
	ApplyOptions( );

	// Each team attacks as often as it defends: an even number of rounds,
	// two at least.

	ServerMissionSettings sms = GetServerSettings();
	if( sms.m_nRounds & 1 )
		sms.m_nRounds++;
	if( sms.m_nRounds <= 2 )
		sms.m_nRounds = 2;
	SetServerSettings(sms);

	if( !g_pMissionButeMgr->Init( MISSION_DM_FILE ))
		return false;

	return CServerMissionMgr::StartGame( msg );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CDemolitionMissionMgr::EndGame
//
//	PURPOSE:	RETAIL: Object 0x100c0cb0: back to the campaign's start
//
// ----------------------------------------------------------------------- //

bool CDemolitionMissionMgr::EndGame( )
{
	if( !CServerMissionMgr::EndGame( ))
		return false;

	if( m_bCustomLevel || m_Campaign.size( ) == 0 )
		return false;

	m_nCurrentLevel = 0;
	m_nCurCampaignIndex = 0;
	m_nCurrentMission = m_Campaign[m_nCurCampaignIndex];
	m_bNewMission = true;

	char const* pszFilename = GetLevelFromMission( m_nCurrentMission, m_nCurrentLevel );
	if( !pszFilename )
		return false;

	return !!ExitLevel( pszFilename, false );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CDemolitionMissionMgr::FinishExitLevel
//
//	PURPOSE:	RETAIL: Object 0x100c0c50
//
// ----------------------------------------------------------------------- //

bool CDemolitionMissionMgr::FinishExitLevel( )
{
	// No longer waiting for exitlevel messages back from client.
	m_bExitingLevel = false;

	// Clear out the playertracker.
	m_PlayerTracker.Term( );

	// Tell the players to handle an exit level.
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayerObj = *iter;
		pPlayerObj->HandleExit( true );
		iter++;
	}

	// Load new level with no restoring of keepalives or save games.
	if( !g_pServerSaveLoadMgr->LoadNewLevel( m_sCurrentWorldName ))
		return false;

	// Unpause the server now that we're done switching levels.
	g_pGameServerShell->PauseGame( LTFALSE );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CDemolitionMissionMgr::LevelStarted
//
//	PURPOSE:	RETAIL: Object 0x100c09a0
//
// ----------------------------------------------------------------------- //

void CDemolitionMissionMgr::LevelStarted()
{
	CServerMissionMgr::LevelStarted( );

	// Check if this is a new level, or just another round.
	if( m_bNewMission )
	{
		CTeamMgr::Instance( ).NewLevel( );
	}
	else
	{
		CTeamMgr::Instance( ).NewRound( );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CDemolitionMissionMgr::HandleMultiplayerOptions
//
//	PURPOSE:	RETAIL: Object 0x100c0bb0
//
// ----------------------------------------------------------------------- //

bool CDemolitionMissionMgr::HandleMultiplayerOptions( HCLIENT hSender, ILTMessage_Read& msg )
{
	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );
	sgo.GetDemolition().Read(&msg);

	ApplyOptions( );

	return true;
}
