// ----------------------------------------------------------------------- //
//
// MODULE  : HUDCarrying.cpp
//
// PURPOSE : HUDItem to display an icon while carrying a body
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "TO2HUDMgr.h"
#include "TO2PlayerStats.h"
#include "TO2InterfaceMgr.h"
#include "TO2PlayerMgr.h"
#include "ChassisButeMgr.h"
#include "ChassisPieceFX.h"
#include "KeyMgr.h"

//******************************************************************************************
//**
//** HUD Carry Icon display
//**
//******************************************************************************************

CHUDCarrying::CHUDCarrying()
{
	m_UpdateFlags = kHUDCarry | kHUDFrame;
	m_hBodyCanDrop = LTNULL;
	m_hBodyNoDrop = LTNULL;
	m_hBodyCanCarry = LTNULL;
}


LTBOOL CHUDCarrying::Init()
{
	m_hBodyCanDrop = g_pInterfaceResMgr->GetTexture("interface\\hud\\carrying.dtx");
	m_hBodyNoDrop = g_pInterfaceResMgr->GetTexture("interface\\hud\\nodrop.dtx");
	m_hBodyCanCarry = g_pInterfaceResMgr->GetTexture("interface\\hud\\carry.dtx");

	// RETAIL: 0x1005244b the chassis pieces' icons, by the pieces' ids.
	ChassisButeMgr::PieceList::const_iterator iter = ChassisButeMgr::Instance( ).GetPieceList( ).begin( );
	for( ; iter != ChassisButeMgr::Instance( ).GetPieceList( ).end( ); ++iter )
	{
		CHASSISPIECE const* pPiece = *iter;

		ChassisPieceIcons icons;
		icons.hCarrying = g_pInterfaceResMgr->GetTexture( pPiece->m_sCarryingIcon.c_str( ));
		icons.hPickup = g_pInterfaceResMgr->GetTexture( pPiece->m_sPickupIcon.c_str( ));
		m_ChassisPieceIcons.push_back( icons );
	}

	g_pDrawPrim->SetRGBA(&m_Poly,argbWhite);
	SetupQuadUVs(m_Poly, m_hBodyCanDrop, 0.0f,0.0f,1.0f,1.0f);

	UpdateLayout();

	return LTTRUE;
}

void CHUDCarrying::Term()
{

}

void CHUDCarrying::Render()
{
	uint8 nCarry = g_pPlayerMgr->GetCarryingObject();
	CSpecialFX* pFX = LTNULL;
	uint8 nCanCarry = g_pPlayerMgr->CanCarryObject( &pFX );
	if (nCarry || nCanCarry)
	{
		float fx = (float)(m_BasePos.x) * g_pInterfaceResMgr->GetXRatio();
		float fy = (float)(m_BasePos.y) * g_pInterfaceResMgr->GetXRatio();
		float fw = (float)(m_nSize) * g_pInterfaceResMgr->GetXRatio();

		SetRenderState();

		g_pDrawPrim->SetTexture(NULL);
		g_pDrawPrim->SetXYWH(&m_Poly,fx,fy,fw,fw);

		if(nCanCarry)
		{
			switch (nCanCarry)
			{
			case CFX_CARRY_BODY:
				g_pDrawPrim->SetTexture(m_hBodyCanCarry);
				break;
			case CFX_CARRY_CHASSISPIECE:
				{
					CHASSISPIECE const* pPiece = dynamic_cast< ChassisPieceFX* >( pFX )->GetChassisPiece( );
					if( pPiece )
						g_pDrawPrim->SetTexture( m_ChassisPieceIcons[pPiece->m_nId].hPickup );
				}
				break;
			};
		}
		else if (g_pPlayerMgr->CanDropCarriedObject())
		{
			switch (nCarry)
			{
			case CFX_CARRY_BODY:
				g_pDrawPrim->SetTexture(m_hBodyCanDrop);
				break;
			case CFX_CARRY_CHASSISPIECE:
				{
					CHASSISPIECE const* pPiece = g_pPlayerMgr->GetCarryingChassisPiece( );
					if( pPiece )
						g_pDrawPrim->SetTexture( m_ChassisPieceIcons[pPiece->m_nId].hCarrying );
				}
				break;
			};
		}
		else
		{
			g_pDrawPrim->SetTexture(m_hBodyNoDrop);
		}

		g_pDrawPrim->DrawPrim(&m_Poly);
	}

	// The keys held, in a row after the carrying icon.
	if( m_KeyIcons.size( ))
	{
		int nX = m_BasePos.x + m_nSize + 2;
		float fy = (float)(m_BasePos.y) * g_pInterfaceResMgr->GetXRatio();

		g_pDrawPrim->SetTexture(NULL);

		for( KeyIconList::iterator iter = m_KeyIcons.begin( ); iter != m_KeyIcons.end( ); ++iter )
		{
			g_pDrawPrim->SetXYWH( &m_Poly, (float)nX * g_pInterfaceResMgr->GetXRatio(), fy,
				(float)iter->nWidth * g_pInterfaceResMgr->GetXRatio(), (float)iter->nHeight * g_pInterfaceResMgr->GetXRatio() );
			g_pDrawPrim->SetTexture( iter->hIcon );
			g_pDrawPrim->DrawPrim( &m_Poly );

			nX += iter->nWidth + 2;
		}
	}
}

void CHUDCarrying::Update()
{
}

void CHUDCarrying::UpdateLayout()
{
	int nCurrentLayout = GetConsoleInt("HUDLayout",0);

	m_BasePos		= g_pLayoutMgr->GetCarryIconPos(nCurrentLayout);
	m_nSize			= g_pLayoutMgr->GetCarryIconSize(nCurrentLayout);

	if (!m_nSize)
	{
		m_BasePos		= LTIntPt(40,360);
		m_nSize			= 64;
	}

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHUDCarrying::RemoveKey
//
// ----------------------------------------------------------------------- //

bool CHUDCarrying::RemoveKey( uint32 nKeyId )
{
	KeyIconList::iterator iter = m_KeyIcons.begin( );
	for( ; iter != m_KeyIcons.end( ); ++iter )
	{
		if( iter->nKeyId == nKeyId )
			break;
	}

	if( iter == m_KeyIcons.end( ))
		return false;

	m_KeyIcons.erase( iter );
	g_pHUDMgr->QueueUpdate( kHUDCarry );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHUDCarrying::ClearKeys
//
// ----------------------------------------------------------------------- //

void CHUDCarrying::ClearKeys( )
{
	m_KeyIcons.clear( );
	g_pHUDMgr->QueueUpdate( kHUDCarry );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHUDCarrying::AddKey
//
//	PURPOSE:	Show a key, its icon scaled to fit the carrying icon's size.
//
// ----------------------------------------------------------------------- //

bool CHUDCarrying::AddKey( uint32 nKeyId )
{
	KeyIcon icon;
	icon.nKeyId = nKeyId;

	for( KeyIconList::iterator iter = m_KeyIcons.begin( ); iter != m_KeyIcons.end( ); ++iter )
	{
		if( iter->nKeyId == nKeyId )
			return false;
	}

	KEY* pKey = g_pKeyMgr->GetKey( nKeyId );
	if( !pKey )
		return false;

	HTEXTURE hIcon = g_pInterfaceResMgr->GetTexture( pKey->szImage );

	uint32 nWidth, nHeight;
	g_pTexInterface->GetTextureDims( hIcon, nWidth, nHeight );

	float fScale;
	if( nWidth > nHeight )
		fScale = (float)m_nSize / (float)nWidth;
	else
		fScale = (float)m_nSize / (float)nHeight;

	icon.hIcon = hIcon;
	icon.nWidth = (int)( (float)nWidth * fScale + 0.5f );
	icon.nHeight = (int)( (float)nHeight * fScale + 0.5f );
	m_KeyIcons.push_back( icon );

	g_pHUDMgr->QueueUpdate( kHUDCarry );

	return true;
}
