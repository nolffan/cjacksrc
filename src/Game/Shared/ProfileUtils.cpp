#include "stdafx.h"
#include "ProfileUtils.h"
#include "ButeMgr.h"
#include "ltbasedefs.h"
#include "ModelButeMgr.h"
#include "CommonUtilities.h"
#include "ResShared.h"
#ifdef _CLIENTBUILD
#include "ClientUtilities.h"
#include "ClientResShared.h"
#endif //_CLIENTBUILD


#define ARRAY_LEN(array) (sizeof((array)) / sizeof((array)[0]))
#define DEFAULT_SESSION ""


// RETAIL: Contract J.A.C.K.'s game options, reconstructed from Object.lto
// (ProfileUtils.obj, 0x1011a900..0x1011c490).

BaseGameOptions* ServerGameOptions::GetCurrentOptions( ) const
{
	ServerGameOptions* pThis = const_cast< ServerGameOptions* >( this );
	switch( m_eGameType )
	{
		case eGameTypeSingle:			return &pThis->m_SP;
		case eGameTypeCooperative:		return &pThis->m_Coop;
		case eGameTypeTeamDeathmatch:	return &pThis->m_TeamDM;
		case eGameTypeDoomsDay:			return &pThis->m_DD;
		case eGameTypeDemolition:		return &pThis->m_Demolition;
		case eGameTypeDeathmatch:
		default:						return &pThis->m_DM;
	}
}

const char*	ServerGameOptions::GetCampaignName() const
{
	return GetCurrentOptions( )->m_sCampaignName.c_str( );
}

const char*	ServerGameOptions::GetSessionName() const
{
	return GetCurrentOptions( )->m_sSessionName.c_str( );
}

uint8 ServerGameOptions::GetMaxPlayers() const
{
	return GetCurrentOptions( )->m_nMaxPlayers;
}

int ServerGameOptions::GetGameTypeStringID() const
{
	switch (m_eGameType)
	{
		case eGameTypeCooperative:		return IDS_COOPERATIVE;
		case eGameTypeDeathmatch:		return IDS_DEATHMATCH;
		case eGameTypeTeamDeathmatch:	return IDS_TEAMDEATHMATCH;
		case eGameTypeDemolition:		return IDS_DEMOLITION;
		case eGameTypeDoomsDay:			return IDS_DOOMSDAY;
		default:						return 0;
	};
}

/************************************************************************************
**** Base game options
************************************************************************************/

void BaseGameOptions::Clear( )
{
	m_sSessionName = "";
	m_sCampaignName = DEFAULT_CAMPAIGN;

	m_nMaxPlayers = 8;
	m_nRunSpeed = 130;
	m_nScoreLimit = 50;
	m_nTimeLimit = 10;
	m_nRounds = 1;
	m_bFriendlyFire = true;
	m_bWeaponsStay = false;

	m_nFragScore = 2;
	m_nRevivingScore = 1;

	m_nNumTeams = 2;

	char szTemp[32] = {0};
	for(int i = 0; i < MAX_TEAMS; ++i )
	{
		m_nTeamModel[i] = i;
		sprintf( szTemp, "Team %i", i+1 );
		m_sTeamName[i] = szTemp;
	}

	m_nDeviceCompletedScore = 10;
	m_nLightPiecePlacedScore = 3;
	m_nHeavyPiecePlacedScore = 5;
	m_nPieceRemovedScore = 2;
}

bool BaseGameOptions::LoadFromBute( CButeMgr& bute )
{
	char const* szTagName = GetName( );
	std::string sAttName = "";

	char szString[256];
	bute.GetString( szTagName, "SessionName", m_sSessionName.c_str( ), szString, ARRAY_LEN( szString ));
	m_sSessionName = szString;

	bute.GetString( szTagName, "CampaignName", DEFAULT_CAMPAIGN, szString, ARRAY_LEN( szString ));
	m_sCampaignName = szString;

	m_nMaxPlayers = (uint8)bute.GetInt( szTagName, "MaxPlayers", m_nMaxPlayers );
	m_nRunSpeed = (uint8)bute.GetInt(szTagName,"RunSpeed",m_nRunSpeed);
	m_nScoreLimit = (uint8)bute.GetInt(szTagName,"ScoreLimit",m_nScoreLimit);
	m_nTimeLimit = (uint8)bute.GetInt(szTagName,"TimeLimit",m_nTimeLimit);
	m_nRounds = (uint8)bute.GetInt( szTagName, "Rounds", m_nRounds );
	m_bFriendlyFire = bute.GetBool(szTagName,"FriendlyFire",m_bFriendlyFire);
	m_bWeaponsStay = bute.GetBool(szTagName,"WeaponsStay",m_bWeaponsStay);
	m_bUseSkills = bute.GetBool(szTagName,"UseSkills",m_bUseSkills);
	m_nDifficulty = (uint8)bute.GetInt(szTagName,"MPDifficulty",m_nDifficulty);
	m_fPlayerDiffFactor = bute.GetFloat(szTagName,"MPPlayerDiffFactor",m_fPlayerDiffFactor);

	m_nFragScore = (uint8)bute.GetInt(szTagName,"FragScore", m_nFragScore);
	m_nRevivingScore = ( uint8 )bute.GetInt( szTagName, "ReviveScore", m_nRevivingScore );

	m_nNumTeams = (uint8)bute.GetInt( szTagName, "NumTeams", m_nNumTeams );

	char szAttName[32] = {0};
	for( int i = 0; i < MAX_TEAMS; ++i )
	{
		sprintf( szAttName, "Team%iModel", i );
		m_nTeamModel[i] = bute.GetInt( szTagName, szAttName, m_nTeamModel[i] );

		sprintf( szAttName, "Team%iName", i );
		bute.GetString( szTagName, szAttName, m_sTeamName[i].c_str(), szString, ARRAY_LEN( szString ));
		m_sTeamName[i] = szString;
	}

	m_nDeviceCompletedScore = (uint8)bute.GetInt(szTagName,"DeviceCompletedScore", m_nDeviceCompletedScore);
	m_nLightPiecePlacedScore = (uint8)bute.GetInt(szTagName,"LightPiecePlacedScore", m_nLightPiecePlacedScore);
	m_nHeavyPiecePlacedScore = (uint8)bute.GetInt(szTagName,"HeavyPiecePlacedScore", m_nHeavyPiecePlacedScore);
	m_nPieceRemovedScore = (uint8)bute.GetInt(szTagName,"PieceRemovedScore", m_nPieceRemovedScore);

	return true;
}

bool BaseGameOptions::SaveToBute( CButeMgr& bute )
{
	char const* szTagName = GetName( );

	bute.SetString( szTagName, "SessionName", m_sSessionName.c_str());
	bute.SetString( szTagName, "CampaignName", m_sCampaignName.c_str( ));

	bute.SetInt(szTagName,"MaxPlayers",(int)m_nMaxPlayers);
	bute.SetInt(szTagName,"RunSpeed",m_nRunSpeed);
	bute.SetInt(szTagName,"ScoreLimit",m_nScoreLimit);
	bute.SetInt(szTagName,"TimeLimit",m_nTimeLimit);
	bute.SetInt(szTagName,"Rounds", m_nRounds );
	bute.SetBool(szTagName,"FriendlyFire",m_bFriendlyFire);
	bute.SetBool(szTagName,"WeaponsStay",m_bWeaponsStay);
	bute.SetBool(szTagName,"UseSkills",m_bUseSkills);
	bute.SetInt(szTagName,"MPDifficulty",m_nDifficulty);
	bute.SetFloat(szTagName,"MPPlayerDiffFactor",m_fPlayerDiffFactor);

	bute.SetInt(szTagName,"FragScore",m_nFragScore);
	bute.SetInt( szTagName, "ReviveScore", m_nRevivingScore );

	bute.SetInt( szTagName, "NumTeams", m_nNumTeams );
	
	char szAttName[32] = {0};
	for( int i = 0; i < MAX_TEAMS; ++i )
	{
		sprintf( szAttName, "Team%iModel", i );
		bute.SetInt( szTagName, szAttName, m_nTeamModel[i] );

		sprintf( szAttName, "Team%iName", i );
		bute.SetString( szTagName, szAttName, m_sTeamName[i].c_str() );
	}

	bute.SetInt(szTagName,"DeviceCompletedScore",m_nDeviceCompletedScore);
	bute.SetInt(szTagName,"LightPiecePlacedScore",m_nLightPiecePlacedScore);
	bute.SetInt(szTagName,"HeavyPiecePlacedScore",m_nHeavyPiecePlacedScore);
	bute.SetInt(szTagName,"PieceRemovedScore",m_nPieceRemovedScore);

	return true;
}

BaseGameOptions& BaseGameOptions::Copy( BaseGameOptions const& other )
{
	if( &other == this )
		return *this;

	m_sSessionName = other.m_sSessionName;
	m_sCampaignName = other.m_sCampaignName;

	// RETAIL: not the skills, the difficulty or the player difficulty factor
	m_nMaxPlayers = other.m_nMaxPlayers;
	m_nRunSpeed = other.m_nRunSpeed;
	m_nScoreLimit = other.m_nScoreLimit;
	m_nTimeLimit = other.m_nTimeLimit;
	m_nRounds = other.m_nRounds;
	m_bFriendlyFire = other.m_bFriendlyFire;
	m_bWeaponsStay = other.m_bWeaponsStay;

	m_nFragScore = other.m_nFragScore;
	m_nRevivingScore = other.m_nRevivingScore;

	m_nNumTeams = other.m_nNumTeams;
	for(int i = 0; i < MAX_TEAMS; ++i )
	{
		m_nTeamModel[i] = other.m_nTeamModel[i];
		m_sTeamName[i] = other.m_sTeamName[i];
	}

	m_nDeviceCompletedScore = other.m_nDeviceCompletedScore;
	m_nLightPiecePlacedScore = other.m_nLightPiecePlacedScore;
	m_nHeavyPiecePlacedScore = other.m_nHeavyPiecePlacedScore;
	m_nPieceRemovedScore = other.m_nPieceRemovedScore;

	return *this;
}

/************************************************************************************
**** The game types' defaults
************************************************************************************/

void CoopGameOptions::Clear( )
{
	BaseGameOptions::Clear( );

	m_nMaxPlayers = 4;
	m_bUseSkills = true;
	m_bFriendlyFire = true;
	m_nDifficulty = 1;
	m_fPlayerDiffFactor = 1.0f;
}

void DMGameOptions::Clear( )
{
	BaseGameOptions::Clear( );

	m_nMaxPlayers = 8;
	m_nRunSpeed = 130;
	m_nScoreLimit = 25;
	m_nTimeLimit = 10;
	m_nRounds = 1;
	m_nFragScore = 1;
}

void TeamDMGameOptions::Clear( )
{
	BaseGameOptions::Clear( );

	m_nMaxPlayers = 8;
	m_nRunSpeed = 130;
	m_nScoreLimit = 50;
	m_nTimeLimit = 10;
	m_nRounds = 1;
	m_bFriendlyFire = true;
	m_nFragScore = 1;
	m_nRevivingScore = 1;
}

void DemolitionGameOptions::Clear( )
{
	BaseGameOptions::Clear( );

	m_nMaxPlayers = 8;
	m_nRunSpeed = 130;
	m_nRounds = 2;
	m_bFriendlyFire = true;
	m_bWeaponsStay = true;
	m_nFragScore = 1;
	m_nRevivingScore = 1;
}

void DoomsdayGameOptions::Clear( )
{
	BaseGameOptions::Clear( );

	m_nMaxPlayers = 8;
	m_nRunSpeed = 130;
	m_nTimeLimit = 20;
	m_nRounds = 1;
	m_bFriendlyFire = true;
	m_nFragScore = 1;

	m_nDeviceCompletedScore = 10;
	m_nLightPiecePlacedScore = 3;
	m_nHeavyPiecePlacedScore = 5;
	m_nPieceRemovedScore = 2;
	m_nRevivingScore = 1;
}

/************************************************************************************
**** Server game options
************************************************************************************/

bool ServerGameOptions::IsDefaultCampaign() const
{
	return (stricmp(GetCampaignName(), DEFAULT_CAMPAIGN ) == 0);
}

void ServerGameOptions::SetCampaignName(const char*	pszName)
{
	GetCurrentOptions( )->m_sCampaignName = pszName;
}

void ServerGameOptions::SetSessionName(const char*	pszName)
{
	GetCurrentOptions( )->m_sSessionName = pszName;
}

BaseGameOptions::BaseGameOptions( )
{
}

// for transmitting options that may change mid game
void DMGameOptions::Write( ILTMessage_Write *pMsg )
{
	pMsg->Writeuint8(m_nRunSpeed);
	pMsg->Writeuint8(m_nScoreLimit);
	pMsg->Writeuint8(m_nTimeLimit);
	pMsg->Writeuint8(m_nRounds );
	pMsg->Writeuint8(m_nFragScore);
	pMsg->Writebool(m_bWeaponsStay);
}

void DMGameOptions::Read( ILTMessage_Read *pMsg )
{
	m_nRunSpeed = pMsg->Readuint8();
	m_nScoreLimit = pMsg->Readuint8();
	m_nTimeLimit = pMsg->Readuint8();
	m_nRounds = pMsg->Readuint8();
	m_nFragScore = pMsg->Readuint8();
	m_bWeaponsStay = pMsg->Readbool();
}

void TeamDMGameOptions::Write( ILTMessage_Write *pMsg )
{
	pMsg->Writeuint8(m_nRunSpeed);
	pMsg->Writeuint8(m_nTimeLimit);
	pMsg->Writeuint8(m_nScoreLimit);
	pMsg->Writeuint8(m_nRounds );
	pMsg->Writebool(m_bFriendlyFire);
	pMsg->Writeuint8(m_nFragScore);
	pMsg->Writebool(m_bWeaponsStay);
}

void TeamDMGameOptions::Read( ILTMessage_Read *pMsg )
{
	m_nRunSpeed = pMsg->Readuint8();
	m_nTimeLimit = pMsg->Readuint8();
	m_nScoreLimit = pMsg->Readuint8();
	m_nRounds = pMsg->Readuint8();
	m_bFriendlyFire = pMsg->Readbool();
	m_nFragScore = pMsg->Readuint8();
	m_bWeaponsStay = pMsg->Readbool();
}

void DemolitionGameOptions::Write( ILTMessage_Write *pMsg )
{
	pMsg->Writeuint8(m_nRunSpeed);
	pMsg->Writeuint8(m_nRounds );
	pMsg->Writebool(m_bFriendlyFire);
	pMsg->Writeuint8(m_nFragScore);
	pMsg->Writebool(m_bWeaponsStay);
}

void DemolitionGameOptions::Read( ILTMessage_Read *pMsg )
{
	m_nRunSpeed = pMsg->Readuint8();
	m_nRounds = pMsg->Readuint8();
	m_bFriendlyFire = pMsg->Readbool();
	m_nFragScore = pMsg->Readuint8();
	m_bWeaponsStay = pMsg->Readbool();
}

void DoomsdayGameOptions::Write( ILTMessage_Write *pMsg )
{
	pMsg->Writeuint8(m_nRunSpeed);
	pMsg->Writeuint8(m_nTimeLimit);
	pMsg->Writeuint8(m_nRounds );
	pMsg->Writebool(m_bFriendlyFire);
	pMsg->Writeuint8(m_nFragScore);
	pMsg->Writeuint8(m_nDeviceCompletedScore);
	pMsg->Writeuint8(m_nLightPiecePlacedScore);
	pMsg->Writeuint8(m_nHeavyPiecePlacedScore);
	pMsg->Writeuint8(m_nPieceRemovedScore);
	pMsg->Writeuint8(m_nRevivingScore);
	pMsg->Writebool(m_bWeaponsStay);
}

void DoomsdayGameOptions::Read( ILTMessage_Read *pMsg )
{
	m_nRunSpeed = pMsg->Readuint8();
	m_nTimeLimit = pMsg->Readuint8();
	m_nRounds = pMsg->Readuint8();
	m_bFriendlyFire = pMsg->Readbool();
	m_nFragScore = pMsg->Readuint8();
	m_nDeviceCompletedScore = pMsg->Readuint8();
	m_nLightPiecePlacedScore = pMsg->Readuint8();
	m_nHeavyPiecePlacedScore = pMsg->Readuint8();
	m_nPieceRemovedScore = pMsg->Readuint8();
	m_nRevivingScore = pMsg->Readuint8();
	m_bWeaponsStay = pMsg->Readbool();
}

ServerGameOptions::ServerGameOptions( )
{
	Clear( );
}

void ServerGameOptions::Clear( )
{
	m_sProfileName = "";

	m_eGameType = eGameTypeCooperative;

	m_sPassword = "";
	m_bUsePassword = false;
	m_sScmdPassword = "";
	m_bAllowScmdCommands = false;

	m_bDedicated = false;
	m_bLANOnly = false;
	m_bPreCacheAssets = true;
	m_bPerformanceTest = false;
	m_nPort = DEFAULT_PORT;
	m_nBandwidthServer = 3;
	m_nBandwidthServerCustom = g_BandwidthServer[m_nBandwidthServer];
	m_setRestrictedWeapons.clear( );
	m_setRestrictedAmmo.clear( );
	m_setRestrictedGear.clear( );
	m_sModName = "";

	m_Coop.Clear();
	m_DM.Clear();
	m_TeamDM.Clear();
	m_DD.Clear();
	m_Demolition.Clear();

	m_nServerStatus = 0;
}

static void StringSetToDelimitedString( StringSet const& sSet, std::string& sDelimitedString, char const* pszDelimiter )
{
	sDelimitedString.clear( );

	// Save set to a string delimted by commas.
	std::string sValue = "";
	StringSet::const_iterator iter = sSet.begin( );
	while( iter != sSet.end( ))
	{
		std::string const& sItem = *iter;
		sDelimitedString += sItem;
		sDelimitedString += pszDelimiter;

		iter++;
	}
}

static void DelimitedStringToStringSet( std::string const& sDelimitedString, StringSet& sSet, char const* pszDelimiter )
{
	sSet.clear( );

	// Find all items delimited by commas.
	uint32 nPos = 0;
	while( nPos < sDelimitedString.size( ))
	{
		uint32 nNextPos = sDelimitedString.find( ',', nPos );
		if( nNextPos == std::string::npos )
			nNextPos = sDelimitedString.size( );

		uint32 nNumChars = nNextPos - nPos;
		std::string sItem = sDelimitedString.substr( nPos, nNumChars );

		// Add the item to the set.
		if( !sItem.empty( ))
		{
			sSet.insert( sItem );
		}


		nPos = nNextPos + 1;
	}
}

bool ServerGameOptions::LoadFromBute( CButeMgr& bute )
{
	char const szTagName[] = "Multiplayer";
	std::string sAttName = "";

	char szString[256];
	
	m_eGameType = ( GameType )bute.GetInt( szTagName,"GameType", eGameTypeCooperative );
	m_bUsePassword = bute.GetBool( szTagName, "UsePassword", false );
	bute.GetString( szTagName, "Password", m_sPassword.c_str( ), szString, ARRAY_LEN( szString ));
	m_sPassword = szString;
	bute.GetString( szTagName, "ScmdPassword", m_sScmdPassword.c_str( ), szString, ARRAY_LEN( szString ));
	m_sScmdPassword = szString;
	m_bAllowScmdCommands = bute.GetBool( szTagName, "AllowScmdCommands", false );

	m_nPort = ( uint16 )bute.GetInt( szTagName,"Port", DEFAULT_PORT );

	m_nBandwidthServer = (uint8)bute.GetInt(szTagName,"BandwidthServer",3);
	m_nBandwidthServerCustom = (uint16)bute.GetInt(szTagName,"BandwidthServerCustom",g_BandwidthServer[3]);
	
	m_bLANOnly = bute.GetBool( szTagName, "LANOnly", false );
	m_bDedicated = bute.GetBool( szTagName, "Dedicated", false );

	std::string sDelimitedString = "";

	sDelimitedString = bute.GetString( szTagName, "WeaponRestrictions", "" );
	DelimitedStringToStringSet( sDelimitedString, m_setRestrictedWeapons, "," );
	sDelimitedString = bute.GetString( szTagName, "AmmoRestrictions", "" );
	DelimitedStringToStringSet( sDelimitedString, m_setRestrictedAmmo, "," );
	sDelimitedString = bute.GetString( szTagName, "GearRestrictions", "" );
	DelimitedStringToStringSet( sDelimitedString, m_setRestrictedGear, "," );

	m_Coop.LoadFromBute( bute );
	m_DM.LoadFromBute( bute );
	m_TeamDM.LoadFromBute( bute );
	m_DD.LoadFromBute( bute );
	m_Demolition.LoadFromBute( bute );

	
	return true;
}

bool ServerGameOptions::SaveToBute( CButeMgr& bute )
{
	char const szTagName[] = "Multiplayer";

	bute.SetInt(szTagName,"GameType", m_eGameType);

	bute.SetBool(szTagName,"UsePassword",m_bUsePassword);
	bute.SetString(szTagName, "Password",m_sPassword.c_str());

	bute.SetString(szTagName, "ScmdPassword",m_sScmdPassword.c_str());
	bute.SetBool(szTagName,"AllowScmdCommands",m_bAllowScmdCommands);

	bute.SetInt(szTagName,"Port",(int)m_nPort);

	bute.SetInt( szTagName, "BandwidthServer", ( int )m_nBandwidthServer );
	bute.SetInt( szTagName, "BandwidthServerCustom", ( int )m_nBandwidthServerCustom );
	
	bute.SetBool( szTagName, "LANOnly", m_bLANOnly );
	bute.SetBool( szTagName, "Dedicated", m_bDedicated );

	std::string sDelimitedString = "";

	StringSetToDelimitedString( m_setRestrictedWeapons, sDelimitedString, "," );
	bute.SetString( szTagName, "WeaponRestrictions", sDelimitedString.c_str( ));
	StringSetToDelimitedString( m_setRestrictedAmmo, sDelimitedString, "," );
	bute.SetString( szTagName, "AmmoRestrictions", sDelimitedString.c_str( ));
	StringSetToDelimitedString( m_setRestrictedGear, sDelimitedString, "," );
	bute.SetString( szTagName, "GearRestrictions", sDelimitedString.c_str( ));

	m_Coop.SaveToBute( bute );
	m_DM.SaveToBute( bute );
	m_TeamDM.SaveToBute( bute );
	m_DD.SaveToBute( bute );
	m_Demolition.SaveToBute( bute );

	return true;
}

ServerGameOptions& ServerGameOptions::Copy( ServerGameOptions const& other )
{
	if( &other == this )
		return *this;

	m_sProfileName = other.m_sProfileName;

	m_eGameType = other.m_eGameType;
	m_bUsePassword = other.m_bUsePassword;
	m_sPassword = other.m_sPassword;
	m_sScmdPassword = other.m_sScmdPassword;
	m_bAllowScmdCommands = other.m_bAllowScmdCommands;

	m_bDedicated = other.m_bDedicated;
	m_bLANOnly = other.m_bLANOnly;
	m_bPreCacheAssets = other.m_bPreCacheAssets;
	m_bPerformanceTest = other.m_bPerformanceTest;

	m_nPort = other.m_nPort;
	m_nBandwidthServer = other.m_nBandwidthServer;
	m_nBandwidthServerCustom = other.m_nBandwidthServerCustom;

	m_setRestrictedWeapons = other.m_setRestrictedWeapons;
	m_setRestrictedAmmo = other.m_setRestrictedAmmo;
	m_setRestrictedGear = other.m_setRestrictedGear;

	m_sModName = other.m_sModName;

	m_Coop.Copy(other.m_Coop);
	m_DM.Copy(other.m_DM);
	m_TeamDM.Copy(other.m_TeamDM);
	m_DD.Copy(other.m_DD);
	m_Demolition.Copy(other.m_Demolition);

	return *this;
}

/***********************************************************************************/

const char*	GetProfileDir( char const* pszProfileName ) 
{
	static std::string directory;
	directory = PROFILE_DIR;
	directory += pszProfileName;
	return directory.c_str();
}

const char*	GetProfileFile( char const* pszProfileName ) 
{
	static std::string sProfileFile;
	sProfileFile = PROFILE_DIR;
	sProfileFile += pszProfileName;
	sProfileFile += PROFILE_EXT;
	return sProfileFile.c_str();
}

static const char szCampaignDirs[g_knNumGameTypes][32] = 
{
	"\\",
	"\\Campaigns\\",
	"\\DM_Campaigns\\",
	"\\TDM_Campaigns\\",
	"\\DD_Campaigns\\",
	"\\DE_Campaigns\\",
};


const char*	GetCampaignDir( char const* pszProfileName, GameType eGameType ) 
{
	ASSERT( eGameType >= 0 && eGameType < g_knNumGameTypes );
	
	static std::string sCampaignDir;
	sCampaignDir = GetProfileDir( pszProfileName );
	sCampaignDir += szCampaignDirs[eGameType]; 

	return sCampaignDir.c_str();
}

const char*	GetCampaignFile( ServerGameOptions const& serverGameOptions ) 
{
	static std::string sCampaignFile;

	if (serverGameOptions.m_eGameType == eGameTypeSingle)
		return "";

	if( serverGameOptions.IsDefaultCampaign() )
	{
		sCampaignFile = GetCampaignDir( serverGameOptions.m_sProfileName.c_str( ), serverGameOptions.m_eGameType );
		sCampaignFile += DEFAULT_CAMPAIGN_FILE;
	}
	else
	{
		sCampaignFile = GetCampaignDir( serverGameOptions.m_sProfileName.c_str( ), serverGameOptions.m_eGameType );
		sCampaignFile += serverGameOptions.GetCampaignName(); 
		sCampaignFile += ".txt";
	}

	return sCampaignFile.c_str();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	FixPlayerName
//
//	PURPOSE:	The player name without leading and trailing whitespace, at
//				most MAX_PLAYER_NAME-1 characters
//
//	RETAIL: ContractJackServer.exe 0x4044c0, CShell.dll 0x100aeba0, Object.lto
//	0x1011b8a0; the name is the one FEAR's ProfileUtils.cpp gives its wide
//	version
//
// ----------------------------------------------------------------------- //

const char*	FixPlayerName(char const* pszPlayerName) 
{
	static char szName[MAX_PLAYER_NAME];

	char *pName = (char *)pszPlayerName;

	//strip leading whitespace
	while (pName && (*pName == ' ' || *pName == '\t'))
	{
		pName++;
	}

	if (pName)
	{
		strncpy(szName, pName, MAX_PLAYER_NAME-1);
		szName[MAX_PLAYER_NAME-1] = 0;

		//strip trailing whitespace
		int nLen = strlen(szName) - 1;
		while (nLen > 0 && (szName[nLen] == ' ' || szName[nLen] == '\t'))
		{
			szName[nLen] = 0;
			nLen--;
		}
	}
	else
		szName[0] = 0;

	return szName;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ServerGameOptions::GetMaxPlayersForBandwidth
//
//	PURPOSE:	The most players the server's bandwidth allows: 32 kbps a
//				player, 2 to 16 players
//
//	RETAIL: ContractJackServer.exe 0x404550, CShell.dll 0x100aec30, Object.lto
//	0x1011b930
//
// ----------------------------------------------------------------------- //

uint8 ServerGameOptions::GetMaxPlayersForBandwidth() const
{
	uint16 nBandwidth;
	if( m_nBandwidthServer >= eBandwidth_Custom )
		nBandwidth = m_nBandwidthServerCustom;
	else
		nBandwidth = g_BandwidthServer[m_nBandwidthServer];

	int nMaxPlayers = nBandwidth / 32;
	return ( uint8 )(( nMaxPlayers <= 2 ) ? 2 : (( nMaxPlayers > 16 ) ? 16 : nMaxPlayers ));
}

