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
#endif

// RETAIL: Contract J.A.C.K.'s game GUID (CShell.dll's data at 0x101504a0)
LTGUID GAMEGUID = 
{ 0xdf560590, 0xd918, 0x4a51, { 0x9c, 0xb1, 0x38, 0xeb, 0xbe, 0x38, 0x79, 0xad } };

static const char* s_szVersion		= "CJ v1.1";

char const GAME_NAME[] = "Contract Jack";

int const GAME_HANDSHAKE_VER_MAJOR = 1;
int const GAME_HANDSHAKE_VER_MINOR = 3;
int const GAME_HANDSHAKE_VER = ((GAME_HANDSHAKE_VER_MAJOR << 8) + GAME_HANDSHAKE_VER_MINOR);

static const uint32 s_nBuildNumber = 193;
static const char* s_szNetVersion	= "1.0.0.3";
static const char* s_szNetGameName	= "contractjack";

const CVersionMgr::TSaveVersion CVersionMgr::kSaveVersion__1_1 = 72;
const CVersionMgr::TSaveVersion CVersionMgr::kSaveVersion__1_2 = 90;
const CVersionMgr::TSaveVersion CVersionMgr::kSaveVersion__1_3 = 124;
const CVersionMgr::TSaveVersion CVersionMgr::kSaveVersion__CurrentBuild = s_nBuildNumber;

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
