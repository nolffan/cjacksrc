// ----------------------------------------------------------------------- //
//
// MODULE  : ChassisButeMgr.cpp
//
// PURPOSE : The chassis pieces of attributes\chassis.txt.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x10017640..0x10017c60) and Object.lto, which both have it.
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "ChassisButeMgr.h"

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHASSISPIECE::CHASSISPIECE / ~CHASSISPIECE
//
// ----------------------------------------------------------------------- //

CHASSISPIECE::CHASSISPIECE()
{
}

CHASSISPIECE::~CHASSISPIECE()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHASSISPIECE::Read
//
//	PURPOSE:	Read one ChassisPiece<n> record (RETAIL: CShell 0x10017740)
//
// ----------------------------------------------------------------------- //

bool CHASSISPIECE::Read( CButeMgr &buteMgr, char const* pszTag, uint8 nId )
{
	if( !pszTag || !pszTag[0] )
		return false;

	m_sName				= buteMgr.GetString( pszTag, "Name", "" );
	m_sPropType			= buteMgr.GetString( pszTag, "PropType", "" );
	m_sPropTypeTarget	= buteMgr.GetString( pszTag, "PropType_Target", "" );
	m_sPropTypeReplaced	= buteMgr.GetString( pszTag, "PropType_Replaced", "" );
	m_sChassisSocket	= buteMgr.GetString( pszTag, "ChassisSocket", "" );
	m_bHeavy			= !!buteMgr.GetBool( pszTag, "Heavy", false );
	m_sRadarType		= buteMgr.GetString( pszTag, "RadarType" );
	m_sCarryingIcon		= buteMgr.GetString( pszTag, "CarryingIcon" );
	m_sPickupIcon		= buteMgr.GetString( pszTag, "PickupIcon" );
	m_sFadeFX			= buteMgr.GetString( pszTag, "FadeFX" );
	m_sSpawnFX			= buteMgr.GetString( pszTag, "SpawnFX" );
	m_sUnspawnFX		= buteMgr.GetString( pszTag, "UnspawnFX" );

	m_nId = nId;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisButeMgr::ChassisButeMgr / ~ChassisButeMgr
//
// ----------------------------------------------------------------------- //

ChassisButeMgr::ChassisButeMgr()
{
	m_bInitialized = false;
}

ChassisButeMgr::~ChassisButeMgr()
{
	Term( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisButeMgr::Instance()
//
//	PURPOSE:	Instatiator of singleton
//
// ----------------------------------------------------------------------- //

ChassisButeMgr& ChassisButeMgr::Instance( )
{
	static ChassisButeMgr sSingleton;
	return sSingleton;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisButeMgr::Init()
//
//	PURPOSE:	Read the pieces (RETAIL: CShell 0x10017a90)
//
// ----------------------------------------------------------------------- //

LTBOOL ChassisButeMgr::Init( const char* szAttributeFile )
{
	if( m_bInitialized )
		return LTFALSE;

	if( !szAttributeFile || !szAttributeFile[0] )
		szAttributeFile = CHASSISBMGR_DEFAULT_FILE;

	if( !Parse( szAttributeFile ))
		return LTFALSE;

	int nPiece = 0;
	char szTag[256];
	LTSNPrintF( szTag, sizeof( szTag ), "%s%d", "ChassisPiece", nPiece );
	while( m_buteMgr.Exist( szTag ))
	{
		CHASSISPIECE* pPiece = debug_new( CHASSISPIECE );
		if( !pPiece || !pPiece->Read( m_buteMgr, szTag, nPiece ))
		{
			debug_delete( pPiece );
			return LTFALSE;
		}

		m_Pieces.push_back( pPiece );

		nPiece++;
		LTSNPrintF( szTag, sizeof( szTag ), "%s%d", "ChassisPiece", nPiece );
	}

	m_buteMgr.Term( );

	m_bInitialized = true;
	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisButeMgr::Term()
//
// ----------------------------------------------------------------------- //

void ChassisButeMgr::Term( )
{
	for( PieceList::iterator iter = m_Pieces.begin( ); iter != m_Pieces.end( ); iter++ )
	{
		debug_delete( *iter );
	}

	m_Pieces.clear( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisButeMgr::GetPiece()
//
//	PURPOSE:	A piece by name (RETAIL: CShell 0x100178c0)
//
// ----------------------------------------------------------------------- //

CHASSISPIECE const* ChassisButeMgr::GetPiece( char const* pszName )
{
	if( !pszName || !pszName[0] )
		return NULL;

	for( PieceList::iterator iter = m_Pieces.begin( ); iter != m_Pieces.end( ); iter++ )
	{
		CHASSISPIECE* pPiece = *iter;
		if( stricmp( pPiece->m_sName.c_str( ), pszName ) == 0 )
			return pPiece;
	}

	return NULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisButeMgr::GetPieceId()
//
// ----------------------------------------------------------------------- //

uint8 ChassisButeMgr::GetPieceId( char const* pszName )
{
	CHASSISPIECE const* pPiece = GetPiece( pszName );
	if( pPiece )
		return pPiece->m_nId;

	return CHASSISBMGR_INVALID_ID;
}
