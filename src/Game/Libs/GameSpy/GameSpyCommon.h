// ----------------------------------------------------------------------- //
//
// MODULE  : GameSpyCommon.h
//
// PURPOSE : What the browser, the patch info request and the server share:
//           each build's GameSpy names, keys and ids, and the query keys.
//
// cjacksrc: Monolith's names are lost; reconstructed from the retail
// GameSpy.dll, 0x10002f20..0x1000342b. The functions are members of a
// singleton (thiscall, though none uses its object).
//
// ----------------------------------------------------------------------- //

#ifndef __GAMESPYCOMMON_H__
#define __GAMESPYCOMMON_H__

#include <string>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

#define LTMIN(a,b) (((a) < (b)) ? (a) : (b))
#define LTMAX(a,b) (((a) > (b)) ? (a) : (b))

// No key id: the reserved key list's and the key registry's failure.
#define GAMESPY_INVALID_KEYID	0xff
// The ids of the keys that are not qr2's reserved ones start here
// (qr2's NUM_RESERVED_KEYS) and end before 0xfe.
#define GAMESPY_FIRST_CUSTOM_KEYID	50
#define GAMESPY_MAX_KEYS			254

class CGameSpyCommon
{
	public:

		CGameSpyCommon( ) { }

		static CGameSpyCommon& Instance( );

		// The gamename the server list is queried for.
		bool GetQueryGameName( int nGameType, char* pszGameName, uint32 nGameNameLen );
		// The gamename the game reports as, and queries from.
		bool GetReportGameName( int nGameType, char* pszGameName, uint32 nGameNameLen );
		// The secret key of the reporting gamename.
		bool GetSecretKey( int nGameType, char* pszSecretKey, uint32 nSecretKeyLen );
		// The product id of patch and usage tracking requests.
		bool GetProductId( int nGameType, int* pnProductId );
		// The game id of the CD key SDK, if the build checks CD keys.
		bool GetCDKeyGameId( int nGameType, int* pnGameId );

		// A key name without its player or team index ("score_12" -> "score").
		bool GetKeyBaseName( char const* pszKeyName, char* pszBaseName, uint32 nBaseNameLen );
		// Is this a key name qr2 accepts: "name", "name_" or "name_t"?
		bool IsValidKeyName( char const* pszKeyName );
		// The qr2 reserved key id of a key name, or GAMESPY_INVALID_KEYID.
		uint8 GetReservedKeyId( char const* pszKeyName );
		// The id for a key: its reserved one, else the next free custom one.
		uint8 GetKeyId( char const* pszKeyName, uint8* pnNextKeyId );
};

// Conversions between narrow and wide strings, one character at a time.
std::wstring ToWideString( std::string const& sString );
void FromWideString( std::wstring const& sWideString, std::string& sString );

#endif // __GAMESPYCOMMON_H__
