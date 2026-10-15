// ----------------------------------------------------------------------- //
//
// MODULE  : ProfileUtils.h
//
// PURPOSE : Profile utilities
//
// (c) 2001-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef PROFILEUTILS_H
#define PROFILEUTILS_H

#include "ltbasetypes.h"
#include "NetDefs.h"
#include "TeamMgr.h"
#include <string>

class CButeMgr;

#define DEFAULT_CAMPAIGN	".default"
#define DEFAULT_CAMPAIGN_FILE	"default_campaign.txt"
#define PROFILE_DIR "Profiles\\"
#define PROFILE_EXT ".txt"

#define MAX_PROFILE_NAME	16


// RETAIL: Contract J.A.C.K.'s game options are one polymorphic class,
// BaseGameOptions, with all the options of every game type, and a subclass
// per game type that names it and gives the defaults; ServerGameOptions has
// one of each, single player's and Demolition's too (the RTTI and vtables of
// Object.lto and CShell.dll; Object.lto's ServerGameOptions constructor,
// 0x1011c1d0, lays them out).

class BaseGameOptions
{
	public:

		BaseGameOptions( );

		BaseGameOptions& Copy( BaseGameOptions const& other );

		// The game type's tag in the profile.
		virtual char const* GetName( ) const = 0;

		virtual void	Clear( );

		bool	LoadFromBute( CButeMgr& bute );
		bool	SaveToBute( CButeMgr& bute );

		// for transmitting options that may change mid game
		virtual void	Write( ILTMessage_Write *pMsg ) {}
		virtual void	Read( ILTMessage_Read *pMsg ) {}

		std::string		m_sSessionName;
		std::string		m_sCampaignName;

		uint8			m_nMaxPlayers;
		bool			m_bFriendlyFire;
		bool			m_bWeaponsStay;
		bool			m_bUseSkills;
		uint8			m_nDifficulty;
		float			m_fPlayerDiffFactor;

		uint8			m_nRunSpeed;
		uint8			m_nScoreLimit;
		uint8			m_nTimeLimit;
		uint8			m_nRounds;

		uint8			m_nFragScore;
		uint8			m_nRevivingScore;

		uint8			m_nNumTeams;	// Will this ever be anything but 2?
		uint32			m_nTeamModel[MAX_TEAMS];
		std::string		m_sTeamName[MAX_TEAMS];

		uint8			m_nDeviceCompletedScore;
		uint8			m_nLightPiecePlacedScore;
		uint8			m_nHeavyPiecePlacedScore;
		uint8			m_nPieceRemovedScore;
};

class SingleplayerGameOptions : public BaseGameOptions
{
	public:

		virtual char const* GetName( ) const { return "Singleplayer"; }
};

class CoopGameOptions : public BaseGameOptions
{
	public:

		virtual char const* GetName( ) const { return "Cooperative"; }
		virtual void	Clear( );
};

class DMGameOptions : public BaseGameOptions
{
	public:

		virtual char const* GetName( ) const { return "Deathmatch"; }
		virtual void	Clear( );
		virtual void	Write( ILTMessage_Write *pMsg );
		virtual void	Read( ILTMessage_Read *pMsg );
};

class TeamDMGameOptions : public BaseGameOptions
{
	public:

		virtual char const* GetName( ) const { return "TeamDM"; }
		virtual void	Clear( );
		virtual void	Write( ILTMessage_Write *pMsg );
		virtual void	Read( ILTMessage_Read *pMsg );
};

class DemolitionGameOptions : public BaseGameOptions
{
	public:

		virtual char const* GetName( ) const { return "Demolition"; }
		virtual void	Clear( );
		virtual void	Write( ILTMessage_Write *pMsg );
		virtual void	Read( ILTMessage_Read *pMsg );
};

class DoomsdayGameOptions : public BaseGameOptions
{
	public:

		virtual char const* GetName( ) const { return "Doomsday"; }
		virtual void	Clear( );
		virtual void	Write( ILTMessage_Write *pMsg );
		virtual void	Read( ILTMessage_Read *pMsg );
};


class ServerGameOptions
{
	public:

		ServerGameOptions( );

		ServerGameOptions( ServerGameOptions const& other )
		{
			Copy( other );
		}

		ServerGameOptions& operator=( ServerGameOptions const& other )
		{
			return Copy( other );
		}

		ServerGameOptions& Copy( ServerGameOptions const& other );

		void	Clear( );

		bool	LoadFromBute( CButeMgr& bute );
		bool	SaveToBute( CButeMgr& bute );

		// Saved data: all game types
		GameType		m_eGameType;
		bool			m_bUsePassword;
		std::string		m_sPassword;
		std::string		m_sScmdPassword;
		bool			m_bAllowScmdCommands;
		uint16			m_nPort;
		uint8			m_nBandwidthServer;
		uint16			m_nBandwidthServerCustom;
		bool			m_bLANOnly;
		bool			m_bDedicated;
		StringSet		m_setRestrictedWeapons;
		StringSet		m_setRestrictedAmmo;
		StringSet		m_setRestrictedGear;
		std::string		m_sModName;

		// The options of the current game type.
		BaseGameOptions*	GetCurrentOptions( ) const;

		//for the current game type
		bool		IsDefaultCampaign() const;
		const char*	GetCampaignName() const;
		void		SetCampaignName(const char*	pszName);
		const char*	GetSessionName() const;
		void		SetSessionName(const char*	pszName);
		
		uint8		GetMaxPlayers() const;
		int			GetGameTypeStringID() const;

		// The most players the server's bandwidth allows.
		uint8		GetMaxPlayersForBandwidth() const;

		// Unsaved data.
		std::string		m_sProfileName;
		bool			m_bPreCacheAssets;
		bool			m_bPerformanceTest;

		// RETAIL: how the game server's start went (OnServerInitialized,
		// Object.lto 0x100df380, sets it: 1 not published, 2 failed, 3
		// started), for its host
		uint32			m_nServerStatus;

		SingleplayerGameOptions&	GetSingleplayer() { return m_SP; }
		CoopGameOptions&		GetCoop() { return m_Coop;	}
		DMGameOptions&			GetDeathmatch() { return m_DM; }
		TeamDMGameOptions&		GetTeamDeathmatch() { return m_TeamDM; }
		DemolitionGameOptions&	GetDemolition() { return m_Demolition; }
		DoomsdayGameOptions&	GetDoomsday() { return m_DD; }

	private:

		SingleplayerGameOptions	m_SP;
		CoopGameOptions		m_Coop;
		DMGameOptions		m_DM;
		TeamDMGameOptions	m_TeamDM;
		DemolitionGameOptions	m_Demolition;
		DoomsdayGameOptions	m_DD;
};



const char*	GetProfileDir( char const* pszProfileName );
const char*	GetProfileFile( char const* pszProfileName );
const char*	GetCampaignDir( char const* pszProfileName, GameType eGameType );
const char*	GetCampaignFile( ServerGameOptions const& serverGameOptions );
const char*	FixPlayerName( char const* pszPlayerName );


#endif // PROFILEUTILS_H

