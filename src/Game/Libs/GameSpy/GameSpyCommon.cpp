// ----------------------------------------------------------------------- //
//
// MODULE  : GameSpyCommon.cpp
//
// PURPOSE : What the browser, the patch info request and the server share:
//           each build's GameSpy names, keys and ids, and the query keys.
//
// cjacksrc: reconstructed from the retail GameSpy.dll, 0x10002f20..
// 0x1000342b.
//
// ----------------------------------------------------------------------- //

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <wchar.h>
#include "IGameSpy.h"
#include "GameSpyCommon.h"

// The keys qr2 reserves, by id (qr2regkeys.h's list); their names are
// matched without player or team indices. RETAIL: .data 0x1001a078, and
// its count 0x1001a0f0.
static char const* s_aszReservedKeys[] =
{
	"",
	"hostname",
	"gamename",
	"gamever",
	"hostport",
	"mapname",
	"gametype",
	"gamevariant",
	"numplayers",
	"numteams",
	"maxplayers",
	"gamemode",
	"teamplay",
	"fraglimit",
	"teamfraglimit",
	"timeelapsed",
	"timelimit",
	"roundtime",
	"roundelapsed",
	"password",
	"groupid",
	"player_",
	"score_",
	"skill_",
	"ping_",
	"team_",
	"deaths_",
	"pid_",
	"team_t",
	"score_t",
};
static uint32 s_nNumReservedKeys = sizeof( s_aszReservedKeys ) / sizeof( s_aszReservedKeys[0] );

// RETAIL: 0x10002f20
bool CGameSpyCommon::GetQueryGameName( int nGameType, char* pszGameName, uint32 nGameNameLen )
{
	switch( nGameType )
	{
		case eGameSpyGameType_Retail:
			strncpy( pszGameName, "contractjack", nGameNameLen );
			pszGameName[nGameNameLen-1] = '\0';
			return true;
		case eGameSpyGameType_Demo:
		case eGameSpyGameType_Preview:
			strncpy( pszGameName, "contractjackd", nGameNameLen );
			pszGameName[nGameNameLen-1] = '\0';
			return true;
	}

	return false;
}

// RETAIL: 0x10002f70
bool CGameSpyCommon::GetReportGameName( int nGameType, char* pszGameName, uint32 nGameNameLen )
{
	switch( nGameType )
	{
		case eGameSpyGameType_Retail:
			strncpy( pszGameName, "contractjack", nGameNameLen );
			pszGameName[nGameNameLen-1] = '\0';
			return true;
		case eGameSpyGameType_Demo:
			strncpy( pszGameName, "contractjackd", nGameNameLen );
			pszGameName[nGameNameLen-1] = '\0';
			return true;
		case eGameSpyGameType_Preview:
			strncpy( pszGameName, "contractjackpr", nGameNameLen );
			pszGameName[nGameNameLen-1] = '\0';
			return true;
	}

	return false;
}

// RETAIL: 0x10002fd0. The key is put together a character at a time, so
// that it is not one string in the image.
bool CGameSpyCommon::GetSecretKey( int nGameType, char* pszSecretKey, uint32 nSecretKeyLen )
{
	std::string sSecretKey;

	switch( nGameType )
	{
		case eGameSpyGameType_Retail:
			sSecretKey = "h";
			sSecretKey += "3";
			sSecretKey += "K";
			sSecretKey += "8";
			sSecretKey += "f";
			sSecretKey += "2";
			break;
		case eGameSpyGameType_Demo:
		case eGameSpyGameType_Preview:
			sSecretKey = "U";
			sSecretKey += "3";
			sSecretKey += "k";
			sSecretKey += "2";
			sSecretKey += "f";
			sSecretKey += "8";
			break;
		default:
			return false;
	}

	strncpy( pszSecretKey, sSecretKey.c_str( ), nSecretKeyLen );
	return true;
}

// RETAIL: 0x100030e0
bool CGameSpyCommon::GetProductId( int nGameType, int* pnProductId )
{
	*pnProductId = 0;

	switch( nGameType )
	{
		case eGameSpyGameType_Retail:
			*pnProductId = 10273;
			return true;
		case eGameSpyGameType_Demo:
			*pnProductId = 10295;
			return true;
		case eGameSpyGameType_Other:
			*pnProductId = 10294;
			return true;
		case eGameSpyGameType_Preview:
			*pnProductId = 10296;
			return true;
	}

	return false;
}

// RETAIL: 0x10003140
bool CGameSpyCommon::GetCDKeyGameId( int nGameType, int* pnGameId )
{
	switch( nGameType )
	{
		case eGameSpyGameType_Retail:
			*pnGameId = 835;
			return true;
	}

	return false;
}

// RETAIL: 0x10003160 (not called in the retail image)
bool CGameSpyCommon::GetKeyBaseName( char const* pszKeyName, char* pszBaseName, uint32 nBaseNameLen )
{
	if( !pszKeyName || !pszKeyName[0] || !pszBaseName )
		return false;

	pszBaseName[0] = '\0';

	// Skip the index at the end.
	uint32 nLen = strlen( pszKeyName );
	while( --nLen )
	{
		if( !pszKeyName[nLen] || !isdigit( pszKeyName[nLen] ))
			break;
	}
	nLen++;

	if( nBaseNameLen < nLen )
		return false;

	strncpy( pszBaseName, pszKeyName, nLen );
	pszBaseName[nLen-1] = '\0';
	return true;
}

// RETAIL: 0x100031e0
bool CGameSpyCommon::IsValidKeyName( char const* pszKeyName )
{
	if( !pszKeyName )
		return false;

	// Only "_" and "_t" may end a key name.
	char const* pszSuffix = strchr( pszKeyName, '_' );
	if( !pszSuffix )
		return true;

	pszSuffix++;
	if( !pszSuffix[0] )
		return true;

	if( pszSuffix[0] != 't' && pszSuffix[0] != 'T' )
		return false;

	bool bValid = ( pszSuffix[1] == '\0' );
	return bValid;
}

// RETAIL: 0x10003220
uint8 CGameSpyCommon::GetReservedKeyId( char const* pszKeyName )
{
	if( !pszKeyName || !pszKeyName[0] )
		return GAMESPY_INVALID_KEYID;

	// Leave out the index at the end.
	uint32 nLen = strlen( pszKeyName );
	while( isdigit( pszKeyName[nLen-1] ))
		nLen--;

	for( uint32 nKey = 0; nKey < s_nNumReservedKeys; nKey++ )
	{
		if( !_strnicmp( s_aszReservedKeys[nKey], pszKeyName, nLen ))
			return ( uint8 )nKey;
	}

	return GAMESPY_INVALID_KEYID;
}

// RETAIL: 0x100032b0
uint8 CGameSpyCommon::GetKeyId( char const* pszKeyName, uint8* pnNextKeyId )
{
	if( !pszKeyName || !pszKeyName[0] )
		return 0;

	if( !IsValidKeyName( pszKeyName ))
		return 0;

	uint8 nKeyId = GetReservedKeyId( pszKeyName );
	if( nKeyId == GAMESPY_INVALID_KEYID )
	{
		if( *pnNextKeyId >= GAMESPY_MAX_KEYS )
			return 0;

		nKeyId = ( *pnNextKeyId )++;
	}

	return nKeyId;
}

// RETAIL: 0x10003300 (not called in the retail image)
std::wstring ToWideString( std::string const& sString )
{
	wchar_t* pwszString = ( wchar_t* )_alloca(( sString.length( ) + 1 ) * sizeof( wchar_t ));
	wchar_t* pwszOut = pwszString;
	char const* pszIn = sString.c_str( );
	wchar_t* pwszEnd = pwszString + sString.length( );
	while( pwszOut != pwszEnd )
	{
		*pwszOut++ = btowc( *pszIn++ );
	}
	*pwszOut = 0;

	std::wstring sWideString( pwszString );
	return sWideString;
}

// RETAIL: 0x10003390 (not called in the retail image)
void FromWideString( std::wstring const& sWideString, std::string& sString )
{
	sString.erase( );

	char* pszString = ( char* )_alloca( sWideString.length( ) + 1 );
	char* pszOut = pszString;
	wchar_t const* pwszIn = sWideString.c_str( );
	char* pszEnd = pszString + sWideString.length( );
	while( pszOut != pszEnd )
	{
		*pszOut++ = ( char )wctob( *pwszIn++ );
	}
	*pszOut = '\0';

	sString = pszString;
}

// RETAIL: 0x10003410
CGameSpyCommon& CGameSpyCommon::Instance( )
{
	static CGameSpyCommon s_GameSpyCommon;
	return s_GameSpyCommon;
}
