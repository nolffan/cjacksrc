// ----------------------------------------------------------------------- //
//
// MODULE  : VehicleWeapon.cpp
//
// PURPOSE : The weapon of a vehicle the player drives.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x1010c810..0x1010cf45).
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "VehicleWeapon.h"
#include "ClientFXMgr.h"
#include "GameClientShell.h"
#include "PlayerViewAttachmentMgr.h"
#include "CommonUtilities.h"

namespace
{
	HMODELANIM const INVALID_ANI = ( static_cast< HMODELANIM >( -1 ) );

	char *ns_szVehicleIdleAnimationBasename = "Idle_";
	char *ns_szVehicleIdlePreFireAnimationName = "IdlePreFire";
	char *ns_szVehicleIdlePostFireAnimationName = "IdlePostFire";
	char *ns_szVehicleIdleFireAnimationName = "IdleFire";
	char *ns_szVehicleIdleFireAnimationBasename = "IdleFire";
	char *ns_szVehicleFullSpeedFireAnimationName = "FullSpeedFire";
	char *ns_szVehicleFullSpeedPostFireAnimationName = "FullSpeedPostFire";
	char *ns_szVehicleFullSpeedPreFireAnimationName = "FullSpeedPreFire";
	char *ns_szVehicleBrakeFireAnimationName = "BrakeFire";
	char *ns_szVehicleBrakePostFireAnimationName = "BrakePostFire";
	char *ns_szVehicleBrakePreFireAnimationName = "BrakePreFire";
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::CVehicleWeapon / ~CVehicleWeapon
//
// ----------------------------------------------------------------------- //

CVehicleWeapon::CVehicleWeapon()
	: CClientWeapon()
	, m_bFullSpeed( false )
	, m_nFullSpeedPreFireAni( INVALID_ANI )
	, m_nFullSpeedPostFireAni( INVALID_ANI )
	, m_bBraking( false )
	, m_nBrakePreFireAni( INVALID_ANI )
	, m_nBrakePostFireAni( INVALID_ANI )
{
	int i;
	for ( i = 0; i < WM_MAX_FIRE_ANIS; ++i )
	{
		m_nFullSpeedFireAnis[ i ] = INVALID_ANI;
	}

	for ( i = 0; i < WM_MAX_FIRE_ANIS; ++i )
	{
		m_nBrakeFireAnis[ i ] = INVALID_ANI;
	}
}

CVehicleWeapon::~CVehicleWeapon()
{
	Term();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::Activate
//
//	PURPOSE:	Set the weapon up on the vehicle's model.
//
// ----------------------------------------------------------------------- //

bool CVehicleWeapon::Activate()
{
	if ( !m_pWeapon || !m_pAmmo ) return false;

	ResetData();

	InitAnimations();

	CreatePVAttachClientFX();
	CreateMuzzleFlash();
	CreateHeatFX();

	g_pPVAttachmentMgr->CreatePVAttachments( m_hObject );

	SetState( W_IDLE );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::SetVehicleModel
//
// ----------------------------------------------------------------------- //

void CVehicleWeapon::SetVehicleModel( HOBJECT hModel )
{
	m_hObject = hModel;
	g_pCommonLT->SetObjectFlags( hModel, OFT_Client, CF_NOTIFYMODELKEYS, CF_NOTIFYMODELKEYS );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::InitAnimations
//
//	PURPOSE:	The vehicle's fire animations: when idle, at full speed and
//				braking.
//
// ----------------------------------------------------------------------- //

void CVehicleWeapon::InitAnimations( bool bAllowSelectOverride )
{
	m_nSelectAni        = INVALID_ANI;
	m_nDeselectAni      = INVALID_ANI;
	m_nReloadAni        = INVALID_ANI;

	m_nAltSelectAni     = INVALID_ANI;
	m_nAltDeselectAni   = INVALID_ANI;
	m_nAltDeselect2Ani  = INVALID_ANI;
	m_nAltReloadAni     = INVALID_ANI;

	m_nPreFireAni       = g_pLTClient->GetAnimIndex( m_hObject, ns_szVehicleIdlePreFireAnimationName );
	m_nPostFireAni      = g_pLTClient->GetAnimIndex( m_hObject, ns_szVehicleIdlePostFireAnimationName );
	m_nFullSpeedPreFireAni = g_pLTClient->GetAnimIndex( m_hObject, ns_szVehicleFullSpeedPreFireAnimationName );
	m_nFullSpeedPostFireAni = g_pLTClient->GetAnimIndex( m_hObject, ns_szVehicleFullSpeedPostFireAnimationName );
	m_nBrakePreFireAni  = g_pLTClient->GetAnimIndex( m_hObject, ns_szVehicleBrakePreFireAnimationName );
	m_nBrakePostFireAni = g_pLTClient->GetAnimIndex( m_hObject, ns_szVehicleBrakePostFireAnimationName );

	char buf[30];
	int i;

	for ( i = 0; i < WM_MAX_IDLE_ANIS; ++i )
	{
		sprintf( buf, "%s%d", ns_szVehicleIdleAnimationBasename, i );
		m_nIdleAnis[ i ] = g_pLTClient->GetAnimIndex( m_hObject, buf );
	}

	for ( i = 0; i < WM_MAX_FIRE_ANIS; ++i )
	{
		if ( i > 0 )
		{
			sprintf( buf, "%s%d", ns_szVehicleIdleFireAnimationBasename, i );
		}
		else
		{
			sprintf( buf, ns_szVehicleIdleFireAnimationName );
		}

		m_nFireAnis[ i ] = g_pLTClient->GetAnimIndex( m_hObject, buf );
	}

	for ( i = 0; i < WM_MAX_FIRE_ANIS; ++i )
	{
		if ( i > 0 )
		{
			sprintf( buf, "%s%d", ns_szVehicleFullSpeedFireAnimationName, i );
		}
		else
		{
			sprintf( buf, ns_szVehicleFullSpeedFireAnimationName );
		}

		m_nFullSpeedFireAnis[ i ] = g_pLTClient->GetAnimIndex( m_hObject, buf );
	}

	for ( i = 0; i < WM_MAX_FIRE_ANIS; ++i )
	{
		if ( i > 0 )
		{
			sprintf( buf, "%s%d", ns_szVehicleBrakeFireAnimationName, i );
		}
		else
		{
			sprintf( buf, ns_szVehicleBrakeFireAnimationName );
		}

		m_nBrakeFireAnis[ i ] = g_pLTClient->GetAnimIndex( m_hObject, buf );
	}

	for ( i = 0; i < WM_MAX_ALTIDLE_ANIS; ++i )
	{
		m_nAltIdleAnis[ i ] = INVALID_ANI;
	}

	for ( i = 0; i < WM_MAX_ALTFIRE_ANIS; ++i )
	{
		m_nAltFireAnis[ i ] = INVALID_ANI;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::PlayFireAnimation
//
// ----------------------------------------------------------------------- //

bool CVehicleWeapon::PlayFireAnimation( bool bResetAni )
{
	return CClientWeapon::PlayFireAnimation( bResetAni );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::IsPreFireAni / GetPreFireAni
//
// ----------------------------------------------------------------------- //

bool CVehicleWeapon::IsPreFireAni( uint32 dwAni ) const
{
	if ( INVALID_ANI == dwAni )
	{
		return false;
	}

	if ( dwAni == m_nPreFireAni || dwAni == m_nFullSpeedPreFireAni || dwAni == m_nBrakePreFireAni )
	{
		return true;
	}

	return false;
}

uint32 CVehicleWeapon::GetPreFireAni() const
{
	if ( m_bBraking )
	{
		return m_nBrakePreFireAni;
	}
	else if ( m_bFullSpeed )
	{
		return m_nFullSpeedPreFireAni;
	}

	return m_nPreFireAni;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::IsFireAni
//
// ----------------------------------------------------------------------- //

bool CVehicleWeapon::IsFireAni( uint32 dwAni, bool bCheckNormalOnly ) const
{
	if ( INVALID_ANI == dwAni )
	{
		return false;
	}

	int i;
	for ( i = 0; i < WM_MAX_FIRE_ANIS; ++i )
	{
		if ( m_nFireAnis[ i ] == dwAni )
		{
			return true;
		}
	}

	for ( i = 0; i < WM_MAX_FIRE_ANIS; ++i )
	{
		if ( m_nFullSpeedFireAnis[ i ] == dwAni )
		{
			return true;
		}
	}

	for ( i = 0; i < WM_MAX_FIRE_ANIS; ++i )
	{
		if ( m_nBrakeFireAnis[ i ] == dwAni )
		{
			return true;
		}
	}

	for ( i = 0; i < WM_MAX_ALTFIRE_ANIS; ++i )
	{
		if ( m_nAltFireAnis[ i ] == dwAni )
		{
			return true;
		}
	}

	if ( !bCheckNormalOnly && (IsPreFireAni( dwAni ) || IsPostFireAni( dwAni )) )
	{
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::GetFireAni
//
// ----------------------------------------------------------------------- //

uint32 CVehicleWeapon::GetFireAni( FireType eFireType ) const
{
	int nNumValid = 0;

	if ( ( ( eFireType == FT_ALT_FIRE ) && CanUseAltFireAnis() ) ||
	     ( m_bUsingAltFireAnis && ( eFireType == FT_NORMAL_FIRE ) ) )
	{
		uint32 dwValidAltFireAnis[ WM_MAX_ALTFIRE_ANIS ];

		for ( int i = 0; i < WM_MAX_ALTFIRE_ANIS; ++i )
		{
			if ( INVALID_ANI != m_nAltFireAnis[ i ] )
			{
				dwValidAltFireAnis[ nNumValid ] = m_nAltFireAnis[ i ];
				++nNumValid;
			}
		}

		if ( nNumValid > 0 )
		{
			return dwValidAltFireAnis[ GetRandom( 0, ( nNumValid - 1 ) ) ];
		}
	}
	else if ( eFireType == FT_NORMAL_FIRE )
	{
		uint32 dwValidFireAnis[ WM_MAX_FIRE_ANIS ];

		for ( int i = 0; i < WM_MAX_FIRE_ANIS; ++i )
		{
			uint32 dwAni = m_bBraking ? m_nBrakeFireAnis[ i ] :
				( m_bFullSpeed ? m_nFullSpeedFireAnis[ i ] : m_nFireAnis[ i ] );

			if ( INVALID_ANI != dwAni )
			{
				dwValidFireAnis[ nNumValid ] = dwAni;
				++nNumValid;
			}
		}

		if ( nNumValid > 0 )
		{
			return dwValidFireAnis[ GetRandom( 0, ( nNumValid - 1 ) ) ];
		}
	}

	return INVALID_ANI;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::IsPostFireAni / GetPostFireAni
//
// ----------------------------------------------------------------------- //

bool CVehicleWeapon::IsPostFireAni( uint32 dwAni ) const
{
	if ( INVALID_ANI == dwAni )
	{
		return false;
	}

	if ( dwAni == m_nPostFireAni || dwAni == m_nFullSpeedPostFireAni || dwAni == m_nBrakePostFireAni )
	{
		return true;
	}

	return false;
}

uint32 CVehicleWeapon::GetPostFireAni() const
{
	if ( m_bBraking )
	{
		return m_nBrakePostFireAni;
	}
	else if ( m_bFullSpeed )
	{
		return m_nFullSpeedPostFireAni;
	}

	return m_nPostFireAni;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CVehicleWeapon::Deactivate
//
//	PURPOSE:	Take the weapon off the vehicle's model.
//
// ----------------------------------------------------------------------- //

void CVehicleWeapon::Deactivate()
{
	SetState( W_INACTIVE );

	for ( CLIENTFX_LINK_NODE* pNode = m_KeyframedClientFX.m_pNext; pNode != NULL; pNode = pNode->m_pNext )
	{
		g_pClientFXMgr->ShutdownClientFX( &pNode->m_Link );
	}

	m_KeyframedClientFX.DeleteList();

	g_pPVAttachmentMgr->RemovePVAttachments();

	RemoveMuzzleFlash();
	RemoveHeatFX();
	RemovePVAttachClientFX();

	KillLoopSound();
}
