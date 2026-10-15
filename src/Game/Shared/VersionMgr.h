// ----------------------------------------------------------------------- //
//
// MODULE  : VersionMgr.h
//
// PURPOSE : Definition of versioning manager
//
// CREATED : 11/16/2000
//
// (c) 2000 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __VERSION_MGR_H__
#define __VERSION_MGR_H__

#include "ltbasetypes.h"
#include "regmgr.h"
#include <string>

extern LTGUID GAMEGUID;

extern char const GAME_NAME[];
extern int const GAME_HANDSHAKE_VER_MAJOR;
extern int const GAME_HANDSHAKE_VER_MINOR;
extern int const GAME_HANDSHAKE_VER;



// RETAIL: Contract J.A.C.K. has no CTO2VersionMgr: CVersionMgr is the one
// version manager, without virtual functions (no RTTI for it in either game
// DLL), its strings in std::strings (its constructor: Object.lto 0x1015cce0,
// CShell.dll 0x1010d040), the CD key in the registry its own business. The
// game server and client shells each hold one.

class CVersionMgr
{
	public :

		CVersionMgr();

		void Update();

		const char* GetVersion();
		const char* GetBuild() { return m_sBuild.c_str(); }
		const uint32 GetSaveVersion();
		const char* GetNetGameName();
		const char* GetNetVersion();
		const char* GetNetRegion() { return m_sNetRegion.c_str(); }

		LTGUID const* GetBuildGuid();

		void GetCDKey( char* pszCDKey, uint32 nLen );
		void SetCDKey( char const* pszCDKey );

#ifdef _CLIENTBUILD
		inline bool	IsLowViolence() const { return m_bLowViolence; }
#endif

		CRegMgr*	GetRegMgr()		{ return &m_regMgr; }

		typedef uint32 TSaveVersion;


		// Save versions.  Use these to compare against the version of the save file.
		static const TSaveVersion	kSaveVersion__1_1;
		static const TSaveVersion	kSaveVersion__1_2;
		static const TSaveVersion	kSaveVersion__1_3;
	private:
		static const TSaveVersion	kSaveVersion__CurrentBuild;
	public:

		// Sets the save version of the current savegame that is loaded.
		void			SetCurrentSaveVersion( TSaveVersion nCurrentSaveVersion ) { m_nCurrentSaveVersion = nCurrentSaveVersion; }
		TSaveVersion	GetCurrentSaveVersion( ) const;

		void			SetLGFlags( uint8 nLGFlags ) { m_nLastLGFlags = nLGFlags; }

	protected :
		CRegMgr		m_regMgr;
		std::string	m_sNetRegion;
		std::string	m_sBuild;
		std::string	m_sVersion;
		bool		m_bLowViolence;

		// Save version of the currently loaded game.
		TSaveVersion	m_nCurrentSaveVersion;

		uint8			m_nLastLGFlags;
};

extern CVersionMgr* g_pVersionMgr;

#endif __VERSION_MGR_H__