// ----------------------------------------------------------------------- //
//
// MODULE  : VehicleButeMgr.cpp
//
// PURPOSE : The vehicles of attributes\VehicleButes.txt.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from Object.lto
// (0x1015bae0..0x1015c9c9) and CShell.dll, which both have it.
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "VehicleButeMgr.h"

#define VBMGR_VEHICLE_TAG		"Vehicle"

CVehicleButeMgr* g_pVehicleButeMgr = LTNULL;

static char s_aTagName[30];

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	VEHICLEBUTES::VEHICLEBUTES / ~VEHICLEBUTES
//
//	PURPOSE:	RETAIL: Object 0x1015bbd0, 0x1015bae0
//
// ----------------------------------------------------------------------- //

VEHICLEBUTES::~VEHICLEBUTES()
{
}

VEHICLEBUTES::VEHICLEBUTES()
:	nId						( VBMGR_INVALID_ID ),
	bTranslucent			( false ),
	fVelocity				( 0.0f ),
	fAcceleration			( 0.0f ),
	fAccelerationTime		( 0.0f ),
	fMaxDeceleration		( 0.0f ),
	fDecelerationTime		( 0.0f ),
	fMaxBrake				( 0.0f ),
	fBrakeTime				( 0.0f ),
	fStoppedPercent			( 0.0f ),
	fMaxSpeedPercent		( 0.0f ),
	fMinTurnAccel			( 0.0f ),
	fTurnRate				( 0.0f ),
	fInitialTurnRate		( 0.0f ),
	fSurfaceSpeedUpPitch	( 0.0f ),
	fSurfaceSlowDownPitch	( 0.0f ),
	fStartMotionPitch		( 0.0f ),
	fStopMotionPitch		( 0.0f ),
	fBrakePitch				( 0.0f ),
	fDecelPitch				( 0.0f ),
	fImpactAngleSensitivity	( 1.0f ),
	vPVOffset				( 0.0f, 0.0f, 0.0f ),
	bCanShowCrosshair		( false ),
	bContourAffectsRoll		( true ),
	nSurfaceVehicleType		( 0xFF )
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	VEHICLEBUTES::Save
//
//	PURPOSE:	RETAIL: Object 0x1015bd60: write the tweakable values back
//
// ----------------------------------------------------------------------- //

void VEHICLEBUTES::Save( CButeMgr &buteMgr )
{
	LTSNPrintF( s_aTagName, ARRAY_LEN( s_aTagName ), "%s%d", VBMGR_VEHICLE_TAG, nId );

	buteMgr.SetDouble( s_aTagName, "Velocity", fVelocity );
	buteMgr.SetDouble( s_aTagName, "Acceleration", fAcceleration );
	buteMgr.SetDouble( s_aTagName, "AccelerationTime", fAccelerationTime );
	buteMgr.SetDouble( s_aTagName, "MaxDeceleration", fMaxDeceleration );
	buteMgr.SetDouble( s_aTagName, "DecelerationTime", fDecelerationTime );
	buteMgr.SetDouble( s_aTagName, "MaxBrake", fMaxBrake );
	buteMgr.SetDouble( s_aTagName, "BrakeTime", fBrakeTime );
	buteMgr.SetDouble( s_aTagName, "StoppedPercent", fStoppedPercent );
	buteMgr.SetDouble( s_aTagName, "MaxSpeedPercent", fMaxSpeedPercent );
	buteMgr.SetDouble( s_aTagName, "MinTurnAccel", fMinTurnAccel );
	buteMgr.SetDouble( s_aTagName, "TurnRate", fTurnRate );
	buteMgr.SetDouble( s_aTagName, "InitialTurnRate", fInitialTurnRate );
	buteMgr.SetDouble( s_aTagName, "SurfaceSpeedUpPitch", fSurfaceSpeedUpPitch );
	buteMgr.SetDouble( s_aTagName, "SurfaceSlowDownPitch", fSurfaceSlowDownPitch );
	buteMgr.SetDouble( s_aTagName, "StartMotionPitch", fStartMotionPitch );
	buteMgr.SetDouble( s_aTagName, "StopMotionPitch", fStopMotionPitch );
	buteMgr.SetDouble( s_aTagName, "BrakePitch", fBrakePitch );
	buteMgr.SetDouble( s_aTagName, "DecelPitch", fDecelPitch );
	buteMgr.SetDouble( s_aTagName, "ImpactAngleSensitivity", fImpactAngleSensitivity );
	buteMgr.SetVector( s_aTagName, "PVOffset", CAVector( vPVOffset.x, vPVOffset.y, vPVOffset.z ));
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	VEHICLEBUTES::Read
//
//	PURPOSE:	RETAIL: Object 0x1015bff0: read one Vehicle<n> record
//
// ----------------------------------------------------------------------- //

bool VEHICLEBUTES::Read( CButeMgr &buteMgr, char const* pszTag )
{
	if( !pszTag )
		return false;

	sName			= buteMgr.GetString( pszTag, "Name", "" );
	sModel			= buteMgr.GetString( pszTag, "Model", "" );
	sPVModel		= buteMgr.GetString( pszTag, "PVModel", "" );
	sStartUpSnd		= buteMgr.GetString( pszTag, "StartUpSnd", "" );
	sIdleSnd		= buteMgr.GetString( pszTag, "IdleSnd", "" );
	sAccelSnd		= buteMgr.GetString( pszTag, "AccelSnd", "" );
	sDecelSnd		= buteMgr.GetString( pszTag, "DecelSnd", "" );
	sBrakeSnd		= buteMgr.GetString( pszTag, "BrakeSnd", "" );
	sTurnOffSnd		= buteMgr.GetString( pszTag, "TurnOffSnd", "" );
	sSlowImpactSnd	= buteMgr.GetString( pszTag, "SlowImpactSnd", "" );
	sMedImpactSnd	= buteMgr.GetString( pszTag, "MedImpactSnd", "" );
	sFastImpactSnd	= buteMgr.GetString( pszTag, "FastImpactSnd", "" );
	sMPLoop			= buteMgr.GetString( pszTag, "MPLoop", "" );
	sVehicleWeapon	= buteMgr.GetString( pszTag, "VehicleWeapon", "" );

	bHasReverse		= !!buteMgr.GetInt( pszTag, "HasReverse", 0 );
	bMotorcycle		= !!buteMgr.GetInt( pszTag, "Motorcycle", 0 );

	blrSkins.Read( &buteMgr, pszTag, "Skin", 64 );
	blrRenderStyles.Read( &buteMgr, pszTag, "RenderStyle", 64 );
	blrPVSkins.Read( &buteMgr, pszTag, "PVSkin", 64 );
	blrPVRenderStyles.Read( &buteMgr, pszTag, "PVRenderStyle", 64 );

	bTranslucent	= buteMgr.GetBool( pszTag, "Translucent", false );

	fVelocity				= (float)buteMgr.GetDouble( pszTag, "Velocity", 0.0 );
	fAcceleration			= (float)buteMgr.GetDouble( pszTag, "Acceleration", 0.0 );
	fAccelerationTime		= (float)buteMgr.GetDouble( pszTag, "AccelerationTime", 0.0 );
	fMaxDeceleration		= (float)buteMgr.GetDouble( pszTag, "MaxDeceleration", 0.0 );
	fDecelerationTime		= (float)buteMgr.GetDouble( pszTag, "DecelerationTime", 0.0 );
	fMaxBrake				= (float)buteMgr.GetDouble( pszTag, "MaxBrake", 0.0 );
	fBrakeTime				= (float)buteMgr.GetDouble( pszTag, "BrakeTime", 0.0 );
	fStoppedPercent			= (float)buteMgr.GetDouble( pszTag, "StoppedPercent", 0.0 );
	fMaxSpeedPercent		= (float)buteMgr.GetDouble( pszTag, "MaxSpeedPercent", 0.0 );
	fMinTurnAccel			= (float)buteMgr.GetDouble( pszTag, "MinTurnAccel", 0.0 );
	fTurnRate				= (float)buteMgr.GetDouble( pszTag, "TurnRate", 0.0 );
	fInitialTurnRate		= (float)buteMgr.GetDouble( pszTag, "InitialTurnRate", 0.0 );
	fSurfaceSpeedUpPitch	= (float)buteMgr.GetDouble( pszTag, "SurfaceSpeedUpPitch", 0.0 );
	fSurfaceSlowDownPitch	= (float)buteMgr.GetDouble( pszTag, "SurfaceSlowDownPitch", 0.0 );
	fStartMotionPitch		= (float)buteMgr.GetDouble( pszTag, "StartMotionPitch", 0.0 );
	fStopMotionPitch		= (float)buteMgr.GetDouble( pszTag, "StopMotionPitch", 0.0 );
	fBrakePitch				= (float)buteMgr.GetDouble( pszTag, "BrakePitch", 0.0 );
	fDecelPitch				= (float)buteMgr.GetDouble( pszTag, "DecelPitch", 0.0 );
	fImpactAngleSensitivity	= (float)buteMgr.GetDouble( pszTag, "ImpactAngleSensitivity", 1.0 );

	vPVOffset = buteMgr.GetVector( pszTag, "PVOffset", CAVector( 0.0, 0.0, 0.0 ));

	bCanShowCrosshair		= !!buteMgr.GetInt( pszTag, "CanShowCrosshair", 0 );
	bContourAffectsRoll		= !!buteMgr.GetInt( pszTag, "ContourAffectsRoll", 0 );
	nSurfaceVehicleType		= (uint8)buteMgr.GetInt( pszTag, "SurfaceVehicleType", -1 );

	sMPFadeFX		= buteMgr.GetString( pszTag, "MPFadeFX", "" );
	sMPSpawnFX		= buteMgr.GetString( pszTag, "MPSpawnFX", "" );
	sMPUnspawnFX	= buteMgr.GetString( pszTag, "MPUnspawnFX", "" );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleButeMgr::GetVehicle()
//
//	PURPOSE:	RETAIL: Object 0x1015c5b0: a vehicle by id
//
// ----------------------------------------------------------------------- //

VEHICLEBUTES* CVehicleButeMgr::GetVehicle( uint8 nId ) const
{
	if( nId < m_VehicleList.size( ))
		return m_VehicleList[nId];

	return LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleButeMgr::GetVehicle()
//
//	PURPOSE:	RETAIL: Object 0x1015c5f0: a vehicle by name
//
// ----------------------------------------------------------------------- //

VEHICLEBUTES* CVehicleButeMgr::GetVehicle( char const* pszName )
{
	if( !pszName )
		return LTNULL;

	VehicleList::iterator iter;
	for( iter = m_VehicleList.begin( ); iter != m_VehicleList.end( ); iter++ )
	{
		if( !_stricmp( (*iter)->sName.c_str( ), pszName ))
			return *iter;
	}

	return LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleButeMgr::WriteVehicleButes()
//
//	PURPOSE:	RETAIL: Object 0x1015c660: write the vehicles' tweakable
//				values back to the file
//
// ----------------------------------------------------------------------- //

bool CVehicleButeMgr::WriteVehicleButes( VEHICLEBUTES* pVehicle )
{
	m_buteMgr.Init( GBM_DisplayError );
	Parse( VBMGR_DEFAULT_FILE );

	if( pVehicle )
	{
		pVehicle->Save( m_buteMgr );
	}
	else
	{
		VehicleList::iterator iter;
		for( iter = m_VehicleList.begin( ); iter != m_VehicleList.end( ); iter++ )
		{
			(*iter)->Save( m_buteMgr );
		}
	}

	bool bRet = m_buteMgr.Save( );
	m_buteMgr.Term( );

	return bRet;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleButeMgr::CVehicleButeMgr / ~CVehicleButeMgr
//
//	PURPOSE:	RETAIL: Object 0x1015c6e0, 0x1015c7b0
//
// ----------------------------------------------------------------------- //

CVehicleButeMgr::CVehicleButeMgr()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleButeMgr::Term()
//
//	PURPOSE:	RETAIL: Object 0x1015c740
//
// ----------------------------------------------------------------------- //

void CVehicleButeMgr::Term( )
{
	g_pVehicleButeMgr = LTNULL;

	VehicleList::iterator iter;
	for( iter = m_VehicleList.begin( ); iter != m_VehicleList.end( ); iter++ )
	{
		debug_delete( *iter );
	}

	m_VehicleList.clear( );
}

// ----------------------------------------------------------------------- //

CVehicleButeMgr::~CVehicleButeMgr()
{
	Term( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleButeMgr::Init()
//
//	PURPOSE:	RETAIL: Object 0x1015c820: read the vehicles
//
// ----------------------------------------------------------------------- //

LTBOOL CVehicleButeMgr::Init( const char* szAttributeFile )
{
	if( g_pVehicleButeMgr || !szAttributeFile ) return LTFALSE;
	if( !Parse( szAttributeFile )) return LTFALSE;

	g_pVehicleButeMgr = this;

	int nNum = 0;
	LTSNPrintF( s_aTagName, ARRAY_LEN( s_aTagName ), "%s%d", VBMGR_VEHICLE_TAG, nNum );

	while( m_buteMgr.Exist( s_aTagName ))
	{
		VEHICLEBUTES* pVehicle = debug_new( VEHICLEBUTES );
		if( !pVehicle || !pVehicle->Read( m_buteMgr, s_aTagName ))
		{
			debug_delete( pVehicle );
			return LTFALSE;
		}

		pVehicle->nId = nNum;
		m_VehicleList.push_back( pVehicle );

		nNum++;
		LTSNPrintF( s_aTagName, ARRAY_LEN( s_aTagName ), "%s%d", VBMGR_VEHICLE_TAG, nNum );
	}

	m_buteMgr.Term( );

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleButeMgr::Instance()
//
//	PURPOSE:	RETAIL: Object 0x1015c990: the singleton
//
// ----------------------------------------------------------------------- //

CVehicleButeMgr& CVehicleButeMgr::Instance( )
{
	static CVehicleButeMgr sSingleton;
	return sSingleton;
}
