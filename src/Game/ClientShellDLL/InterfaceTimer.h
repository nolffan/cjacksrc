// ----------------------------------------------------------------------- //
//
// MODULE  : InterfaceTimer.h
//
// PURPOSE : Definition of InterfaceTimer class
//
// CREATED : 10/18/99
//
// (c) 1999 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __INTERFACE_TIMER_H__
#define __INTERFACE_TIMER_H__

#include "ltbasedefs.h"
#include "iltfontmanager.h"
#include "TeamMgr.h"
#include "iltclient.h"

extern ILTClient* g_pLTClient;

extern ILTFontManager*	g_pFontManager;

// RETAIL: Contract J.A.C.K.'s timer (CShell 0x1006e2d0) takes its font,
// size, position, colour and alignment from Init and keeps the time left,
// so a paused timer shows the time it stopped at; it no longer knows its
// team.
class CInterfaceTimer
{
	public:

		CInterfaceTimer();
		~CInterfaceTimer();

		bool		Init( uint8 nFont, uint8 nBaseSize, LTIntPt const& pos, LTVector const& vColor,
						  CUI_ALIGNMENTTYPE eAlign );

		void		Draw();
		void		ScreenDimsChanged();

		void        SetTime(LTFLOAT fTime, bool bPause)
		{
			m_fTime = fTime;
			m_fTimeLeft = m_fTime - g_pLTClient->GetGameTime( );
			m_bPause = bPause;
		}
		float		GetTime() const { return m_fTime; }
		float		GetTimeLeft() const { return m_fTimeLeft; }

	private:

		// Server game time when timer runs out.
		float					m_fTime;
		// Time left when last drawn (or when paused).
		float					m_fTimeLeft;
		bool					m_bPause;

		CUIFormattedPolyString	*m_pTimeStr;
		CUIFont					*m_pFont;

		LTIntPt			m_BasePos;
		uint8			m_nBaseSize;
		uint32			m_nColor;
		CUI_ALIGNMENTTYPE	m_eAlign;
};

#endif  // __INTERFACE_TIMER_H__