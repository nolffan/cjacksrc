// ----------------------------------------------------------------------- //
//
// MODULE  : InterfaceTimer.cpp
//
// PURPOSE : Implementation of InterfaceTimer class
//
// CREATED : 10/18/99
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include <stdio.h>
#include "InterfaceTimer.h"
#include "GameClientShell.h"
#include "iltclient.h"
#include "InterfaceResMgr.h"

CInterfaceTimer::CInterfaceTimer() 
{
	m_fTime		= 0.0f;
	m_fTimeLeft	= 0.0f;
	m_bPause	= false;
	m_pTimeStr	= LTNULL;
	m_nBaseSize = 0;
	m_pFont		= LTNULL;
	m_nColor	= argbWhite;
	m_eAlign	= CUI_HALIGN_CENTER;
}

CInterfaceTimer::~CInterfaceTimer() 
{ 
	if (m_pTimeStr) 
		g_pFontManager->DestroyPolyString(m_pTimeStr); 
}

// RETAIL: 0x1006e300
bool CInterfaceTimer::Init( uint8 nFont, uint8 nBaseSize, LTIntPt const& pos, LTVector const& vColor,
						    CUI_ALIGNMENTTYPE eAlign )
{
	m_pFont = g_pInterfaceResMgr->GetFont(nFont);
	if (!m_pFont)
		return false;

	m_nBaseSize = nBaseSize;
	m_BasePos = pos;
	if (m_BasePos.x == 0)
		m_BasePos = LTIntPt(320,40);

	m_nColor = SET_ARGB( 0xFF, ( uint8 )vColor.x, ( uint8 )vColor.y, ( uint8 )vColor.z );
	m_eAlign = eAlign;

	return true;
}

// RETAIL: 0x1006e3a0
void CInterfaceTimer::Draw()
{
	if (!m_bPause)
		m_fTimeLeft = m_fTime - g_pLTClient->GetGameTime( );

	if( m_fTimeLeft <= 0.0f )
		return;

	int nMinutes = int(m_fTimeLeft) / 60;
	int nSeconds = m_fTimeLeft > 60.0 ? int(m_fTimeLeft) % 60 : int(m_fTimeLeft);

	char aBuffer[8];
	sprintf(aBuffer, "%02d:%02d", nMinutes, nSeconds);

 	if (!m_pTimeStr)
 	{
		m_pTimeStr = g_pFontManager->CreateFormattedPolyString(m_pFont, aBuffer);
		m_pTimeStr->SetAlignmentH(m_eAlign);
		m_pTimeStr->SetCharScreenHeight((uint8)((LTFLOAT)m_nBaseSize * g_pInterfaceResMgr->GetXRatio()));
 	}
 	else
 	{
		m_pTimeStr->SetText(aBuffer);
 	}

	float x = (float)m_BasePos.x * g_pInterfaceResMgr->GetXRatio();
	float y = (float)m_BasePos.y * g_pInterfaceResMgr->GetYRatio();
#ifdef CJACK_MODERN
	x += (float)g_pInterfaceResMgr->GetHUDXOffset(m_BasePos.x);	// MODERN.md CL-30
#endif

	m_pTimeStr->SetPosition( x+2.0f, y+2.0f);
	m_pTimeStr->SetColor(argbBlack);
	m_pTimeStr->Render();

	m_pTimeStr->SetPosition( x, y );
	m_pTimeStr->SetColor(m_nColor);
	m_pTimeStr->Render();
}

// RETAIL: 0x1006e500
void CInterfaceTimer::ScreenDimsChanged()
{
	if (m_pTimeStr)
		m_pTimeStr->SetCharScreenHeight((uint8)((LTFLOAT)m_nBaseSize * g_pInterfaceResMgr->GetXRatio()));
}
