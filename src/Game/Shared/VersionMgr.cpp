// ----------------------------------------------------------------------- //
//
// MODULE  : VersionMgr.cpp
//
// PURPOSE : Implementation of versioning manager
//
// CREATED : 11/16/2000
//
// (c) 2000-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "VersionMgr.h"
#include "regmgr.h"
#include "ClientServerShared.h"
#if defined(_CLIENTBUILD)
#include "ClientResShared.h"
#include "ClientRes.h"	// RETAIL: IDS_CJ_1432, the menu's version
#endif

// RETAIL: Contract J.A.C.K.'s game GUID (CShell.dll's data at 0x101504a0)
LTGUID GAMEGUID = 
{ 0xdf560590, 0xd918, 0x4a51, { 0x9c, 0xb1, 0x38, 0xeb, 0xbe, 0x38, 0x79, 0xad } };

static const char* s_szVersion		= "CJ v1.1";

char const GAME_NAME[] = "Contract Jack";

int const GAME_HANDSHAKE_VER_MAJOR = 1;
// RETAIL: 1.0, NOLF2's was 1.3 (GAME_HANDSHAKE_VER is 0x100 in Object.lto's
// .rdata at 0x101ca46c and CShell.dll's at 0x1013a744)
int const GAME_HANDSHAKE_VER_MINOR = 0;
int const GAME_HANDSHAKE_VER = ((GAME_HANDSHAKE_VER_MAJOR << 8) + GAME_HANDSHAKE_VER_MINOR);

static const uint32 s_nBuildNumber = 193;
#if defined(_CLIENTBUILD)
static const char* s_szNetVersion	= "1.0.0.3";
static const char* s_szNetGameName	= "contractjack";
#endif

const CVersionMgr::TSaveVersion CVersionMgr::kSaveVersion__1_1 = 72;
const CVersionMgr::TSaveVersion CVersionMgr::kSaveVersion__1_2 = 90;
const CVersionMgr::TSaveVersion CVersionMgr::kSaveVersion__1_3 = 124;
const CVersionMgr::TSaveVersion CVersionMgr::kSaveVersion__CurrentBuild = s_nBuildNumber;

// RETAIL: every Contract J.A.C.K. binary that has CVersionMgr -- the
// launcher, the dedicated server, CShell.dll and Object.lto -- exports an
// int named by a GUID, {33560E70-C1C2-4DCC-A206-454C1A563E3D}
// (?_0x33560e700xc1c20x4dcc0xa20x60x450x4c0x1a0x560x3e0x3d@@3HA), zero,
// beside g_pVersionMgr (ContractJack.exe .data 0x4113cc). Nothing in the
// binaries reads it: a marker for something outside them to find.
#define VERSIONMGR_EXPORT_GUID( a, b, c, d0, d1, d2, d3, d4, d5, d6, d7 ) \
	__declspec( dllexport ) int _##a##b##c##d0##d1##d2##d3##d4##d5##d6##d7
VERSIONMGR_EXPORT_GUID( 0x33560e70, 0xc1c2, 0x4dcc, 0xa2, 0x6, 0x45, 0x4c, 0x1a, 0x56, 0x3e, 0x3d );

CVersionMgr* g_pVersionMgr = NULL;


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVersionMgr::GetCurrentSaveVersion
//
//	PURPOSE:	Gets the save version of the currently loaded game.
//
//	RETAIL: Object.lto 0x1015cc40, CShell.dll 0x1010cf50
//
// ----------------------------------------------------------------------- //

CVersionMgr::TSaveVersion CVersionMgr::GetCurrentSaveVersion() const
{
	// The only time we care about old save versions is when we are restoring a
	// previously saved game.

	if( m_nLastLGFlags == LOAD_RESTORE_GAME )
		return m_nCurrentSaveVersion;

	return CVersionMgr::kSaveVersion__CurrentBuild;
}

// RETAIL: Object.lto 0x1015cc60, CShell.dll 0x1010cf70
void CVersionMgr::GetCDKey( char* pszCDKey, uint32 nLen )
{
	m_regMgr.Get( "CDKey", pszCDKey, nLen, "" );
}

// RETAIL: Object.lto 0x1015cc80, CShell.dll 0x1010cf90
void CVersionMgr::SetCDKey( char const* pszCDKey )
{
	m_regMgr.Set( "CDKey", pszCDKey ? pszCDKey : "" );
}

// RETAIL: Object.lto 0x1015cca0, CShell.dll 0x1010cfb0
const uint32 CVersionMgr::GetSaveVersion()
{
	return s_nBuildNumber;
}

// RETAIL: Object.lto 0x1015ccb0, ContractJack.exe 0x40a890: an empty member
// function of two arguments between GetSaveVersion and GetVersion; what it
// was is not known (our name, as its arguments' types)
void CVersionMgr::Unknown( uint32 /*nArg1*/, uint32 /*nArg2*/ )
{
}

// RETAIL: Object.lto 0x1015ccc0, CShell.dll 0x1010cfc0
const char* CVersionMgr::GetVersion()
{
	return s_szVersion;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVersionMgr::Update
//
//	RETAIL: Object.lto 0x1015ccd0 (empty), CShell.dll 0x1010cfd0
//
// ----------------------------------------------------------------------- //

void CVersionMgr::Update()
{
#if defined(_CLIENTBUILD)
	if (stricmp(LoadTempString(IDS_ALLOW_GORE),"TRUE") != 0)
	{
		m_bLowViolence = true;
	}

	//override for debugging purposes
	float fVal = 0.0f;

    if (!g_pLTClient) return;

    HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("LowViolence");
	if(hVar)
	{
        fVal = g_pLTClient->GetVarValueFloat(hVar);
		if (fVal >= 1.0f)
		{
			m_bLowViolence = true;
		}

	}
#endif
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVersionMgr::CVersionMgr
//
//	RETAIL: Object.lto 0x1015cce0, CShell.dll 0x1010d040
//
// ----------------------------------------------------------------------- //

CVersionMgr::CVersionMgr()
{
	m_bLowViolence	= false;

	m_regMgr.Init("Monolith Productions", GAME_NAME, "1.0");

	char szTemp[256];
	uint32 nBufSize = sizeof(szTemp);
	m_regMgr.Get("NetRegionCode", szTemp, nBufSize, "EN");
	m_sNetRegion = szTemp;

	sprintf( szTemp, "Build %d", s_nBuildNumber );
	m_sBuild = szTemp;

	m_sVersion = s_szVersion;

	m_nCurrentSaveVersion = 0;

	g_pVersionMgr = this;
}

// NOLF2's, which Contract J.A.C.K. has no use for (WON's server directory
// and the build GUID of the cheat codes): to be taken out with their users.
// Only the client still has them: the launcher and the dedicated server,
// which compile this file without /Gy, have none (ContractJack.exe goes from
// GetVersion's 0x40a8a0 and Update's 0x40a8b0 to the constructor 0x40a8c0).
#if defined(_CLIENTBUILD)
// RETAIL: CShell.dll 0x1010d130: the version the main menu shows, from the
// string table ("v1.1"); GetVersion's "CJ v1.1" is the save files' and the
// servers'
void CVersionMgr::GetDisplayVersion( char* pszVersion, uint32 nLen )
{
	char const* pszDisplay = LoadTempString( IDS_CJ_1432 );
	if( nLen )
	{
		strncpy( pszVersion, pszDisplay, nLen - 1 );
		pszVersion[nLen - 1] = '\0';
	}
}

const char* CVersionMgr::GetNetVersion()
{
	return s_szNetVersion;
}

const char* CVersionMgr::GetNetGameName()
{
	return s_szNetGameName;
}

const LTGUID* CVersionMgr::GetBuildGuid()
{
	return &GAMEGUID;
}
#endif // _CLIENTBUILD
