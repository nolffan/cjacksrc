// ----------------------------------------------------------------------- //
//
// MODULE  : HUDWeaponHeat.cpp
//
// PURPOSE : HUDItem to display the current weapon's heat
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x100621e0..0x100625ae).
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "TO2HUDMgr.h"
#include "TO2PlayerStats.h"
#include "TO2InterfaceMgr.h"
#include "TO2PlayerMgr.h"
#include "ClientWeaponBase.h"
#include "ClientWeaponMgr.h"
#include "CMoveMgr.h"
#include "VehicleMgr.h"

CHUDWeaponHeat::CHUDWeaponHeat()
{
	m_UpdateFlags = kHUDWeaponHeat | kHUDFrame;
}

LTBOOL CHUDWeaponHeat::Init()
{
	UpdateLayout();

	int nCurrentLayout = GetConsoleInt( "HUDLayout", 0 );

	char szTex[256];
	g_pLayoutMgr->GetWeaponHeatTex( nCurrentLayout, szTex, sizeof( szTex ));
	if( !szTex[0] )
		return LTFALSE;

	m_Bar.Init( g_pInterfaceResMgr->GetTexture( szTex ));

	return LTTRUE;
}

void CHUDWeaponHeat::Term()
{
}

void CHUDWeaponHeat::Render()
{
	if( !m_bDraw )
		return;

	SetRenderState();
	m_Bar.Render();
}

void CHUDWeaponHeat::Update()
{
	// RETAIL: 0x10062280: a vehicle's own weapon comes first when the
	// player rides one.
	IClientWeaponBase *pClientWeapon;
	if( g_pMoveMgr->GetVehicleMgr( )->IsVehiclePhysics( ) && g_pMoveMgr->GetVehicleMgr( )->HasVehicleWeapon( ))
		pClientWeapon = g_pMoveMgr->GetVehicleMgr( )->GetVehicleWeapon( );
	else
		pClientWeapon = g_pPlayerMgr->GetClientWeaponMgr( )->GetCurrentClientWeapon( );
	if( !pClientWeapon )
		return;

	m_bDraw = ( pClientWeapon->GetWeapon( )->fHeatFactor > 0.0f );
	if( !m_bDraw )
		return;

	float fHeat = pClientWeapon->GetHeat( );

	float x = (float)m_BasePos.x * g_pInterfaceResMgr->GetXRatio( );
	float y = (float)m_BasePos.y * g_pInterfaceResMgr->GetYRatio( );
	float fScale = m_fBarScale * g_pInterfaceResMgr->GetXRatio( );
	float fFill = fHeat * fScale * 100.0f;
	float fMax = fScale * 100.0f;
	float h = (float)m_nBarHeight * g_pInterfaceResMgr->GetYRatio( );

	if( fHeat < 0.5f )
	{
		LTVector vColor( m_vCool.x + ( m_vWarm.x - m_vCool.x ) * fHeat * 2.0f,
						 m_vCool.y + ( m_vWarm.y - m_vCool.y ) * fHeat * 2.0f,
						 m_vCool.z + ( m_vWarm.z - m_vCool.z ) * fHeat * 2.0f );
		m_Bar.SetColor( SET_ARGB( 0xFF, (uint8)vColor.x, (uint8)vColor.y, (uint8)vColor.z ));
	}
	else if( fHeat < 1.0f )
	{
		float f = ( fHeat - 0.5f ) * 2.0f;
		LTVector vColor( m_vWarm.x + ( m_vHot.x - m_vWarm.x ) * f,
						 m_vWarm.y + ( m_vHot.y - m_vWarm.y ) * f,
						 m_vWarm.z + ( m_vHot.z - m_vWarm.z ) * f );
		m_Bar.SetColor( SET_ARGB( 0xFF, (uint8)vColor.x, (uint8)vColor.y, (uint8)vColor.z ));
	}
	else
	{
		m_Bar.SetColor( m_nOverheatColor );
	}

	m_Bar.Update( x, y, fFill, fMax, h );
}

void CHUDWeaponHeat::UpdateLayout()
{
	int nCurrentLayout = GetConsoleInt( "HUDLayout", 0 );

	m_BasePos		= g_pLayoutMgr->GetWeaponHeatBasePos( nCurrentLayout );
#ifdef CJACK_MODERN
	m_BasePos.x += g_pInterfaceResMgr->GetHUDBaseShift(m_BasePos.x);	// MODERN.md CL-30
#endif
	m_nBarHeight	= g_pLayoutMgr->GetWeaponHeatBarHeight( nCurrentLayout );
	m_fBarScale		= g_pLayoutMgr->GetWeaponHeatBarScale( nCurrentLayout );

	LTVector vOverheat;
	g_pLayoutMgr->GetWeaponHeatColors( nCurrentLayout, &m_vCool, &m_vWarm, &m_vHot, &vOverheat );
	m_nOverheatColor = SET_ARGB( 0xFF, (uint8)vOverheat.x, (uint8)vOverheat.y, (uint8)vOverheat.z );
}
