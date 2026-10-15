// ----------------------------------------------------------------------- //
//
// MODULE  : HUDBombTarget.cpp
//
// PURPOSE : HUDItem to display the demolition bombs' timers beside the
//           radar
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x10051290..0x10051830).
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "TO2HUDMgr.h"
#include "TO2PlayerStats.h"
#include "TO2InterfaceMgr.h"
#include "TO2PlayerMgr.h"
#include "ClientWeaponBase.h"

static char* s_pszBombTag = "Miscellaneous";

#define BT_NUM_BOMBS	3

CHUDBombTarget::CHUDBombTarget()
{
	m_eLevel = kHUDRenderDead;
	m_UpdateFlags = kHUDBombTarget;
	m_Timers.resize( BT_NUM_BOMBS );
	m_fScale = 1.0f;
}

LTBOOL CHUDBombTarget::Init()
{
	UpdateLayout();

	m_Timers[0].m_hIcon = g_pInterfaceResMgr->GetTexture( "interface\\hud\\radar_bomb1.dtx" );
	m_Timers[1].m_hIcon = g_pInterfaceResMgr->GetTexture( "interface\\hud\\radar_bomb2.dtx" );
	m_Timers[2].m_hIcon = g_pInterfaceResMgr->GetTexture( "interface\\hud\\radar_bomb3.dtx" );

	float x = (float)m_BasePos.x * g_pInterfaceResMgr->GetXRatio( );
	float y = (float)m_BasePos.y * g_pInterfaceResMgr->GetXRatio( );
	float fSize = (float)m_nBaseSize * g_pInterfaceResMgr->GetXRatio( );
	float fStep = fSize * 1.25f;

	uint8 nFont = (uint8)g_pLayoutMgr->GetInt( "Miscellaneous", "BombTimerFont" );
	uint8 nSize = (uint8)g_pLayoutMgr->GetInt( "Miscellaneous", "BombTimerSize" );
	LTVector vColor = g_pLayoutMgr->GetVector( "Miscellaneous", "BombTimerColor" );

	LTIntPt pos( m_BasePos.x + m_nBaseSize + 2, m_BasePos.y - 2 );

	for( uint32 i = 0; i < m_Timers.size( ); i++ )
	{
		BombTimer& bomb = m_Timers[i];

		g_pDrawPrim->SetRGBA( &bomb.m_Poly, argbWhite );
		SetupQuadUVs( bomb.m_Poly, bomb.m_hIcon, 0.0f, 0.0f, 1.0f, 1.0f );

		bomb.m_Timer.Init( nFont, nSize, pos, vColor, CUI_HALIGN_LEFT );
		pos.y += (int)( fStep / g_pInterfaceResMgr->GetXRatio( ) + 0.5f );

		g_pDrawPrim->SetXYWH( &bomb.m_Poly, x, y, fSize, fSize );
		y += fStep;
	}

	return LTTRUE;
}

void CHUDBombTarget::Term()
{
}

void CHUDBombTarget::Render()
{
	if( !g_pRadar->GetDraw( ))
		return;

	bool bAnyTimer = false;
	uint32 i;
	for( i = 0; i < m_Timers.size( ); i++ )
	{
		if( m_Timers[i].m_Timer.GetTime( ) > 0.0f )
			bAnyTimer = true;
	}

	if( !bAnyTimer )
		return;

	SetRenderState();

	for( i = 0; i < m_Timers.size( ); i++ )
	{
		BombTimer& bomb = m_Timers[i];
		if( bomb.m_Timer.GetTimeLeft( ) > 0.0f )
		{
			g_pDrawPrim->SetTexture( bomb.m_hIcon );
			g_pDrawPrim->DrawPrim( &bomb.m_Poly, 1 );
			bomb.m_Timer.Draw( );
		}
	}
}

void CHUDBombTarget::Update()
{
	if( m_fScale == g_pInterfaceResMgr->GetXRatio( ))
		return;

	m_fScale = g_pInterfaceResMgr->GetXRatio( );

	uint32 i;
	for( i = 0; i < m_Timers.size( ); i++ )
	{
		m_Timers[i].m_Timer.ScreenDimsChanged( );
	}

	float x = (float)m_BasePos.x * m_fScale;
	float y = (float)m_BasePos.y * m_fScale;
	float fSize = (float)m_nBaseSize * m_fScale;
	float fStep = fSize * 1.25f;

	for( i = 0; i < m_Timers.size( ); i++ )
	{
		g_pDrawPrim->SetXYWH( &m_Timers[i].m_Poly, x, y, fSize, fSize );
		y += fStep;
	}
}

void CHUDBombTarget::UpdateLayout()
{
	m_BasePos = g_pLayoutMgr->GetPoint( s_pszBombTag, "BombTimerPos" );
	m_nBaseSize = (uint8)g_pLayoutMgr->GetInt( s_pszBombTag, "BombTimerSize" );
	if( !m_nBaseSize )
	{
		m_BasePos = LTIntPt( 500, 10 );
		m_nBaseSize = 32;
	}
}

bool CHUDBombTarget::SetTimer( uint8 nTimer, float fTime, bool bPause )
{
	if( nTimer > m_Timers.size( ))
		return false;

	m_Timers[nTimer].m_Timer.SetTime( fTime, bPause );
	return true;
}

float CHUDBombTarget::GetTimer( uint8 nTimer )
{
	if( nTimer > m_Timers.size( ))
		return 0.0f;

	return m_Timers[nTimer].m_Timer.GetTime( );
}
