// ----------------------------------------------------------------------- //
//
// MODULE  : HUDBombTarget.h
//
// PURPOSE : HUDItem to display the demolition bombs' timers beside the
//           radar
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x10051290..0x10051830).
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_BOMBTARGET_H
#define __HUD_BOMBTARGET_H

#include "HUDItem.h"
#include "InterfaceTimer.h"

class CHUDBombTarget : public CHUDItem
{
public:
	CHUDBombTarget();

	LTBOOL		Init();
	void		Term();

	void		Render();
	void		Update();

	void		UpdateLayout();

	// Set a bomb's timer (the display-timer message's types 3 and up).
	bool		SetTimer( uint8 nTimer, float fTime, bool bPause );
	float		GetTimer( uint8 nTimer );

private:

	// One bomb: its timer, and its icon beside it.
	struct BombTimer
	{
		BombTimer( )
		{
			memset( &m_Poly, 0, sizeof( m_Poly ));
			m_hIcon = LTNULL;
		}

		CInterfaceTimer	m_Timer;
		LTPoly_GT4		m_Poly;
		HTEXTURE		m_hIcon;
	};

	typedef std::vector< BombTimer > BombTimerList;

	LTIntPt			m_BasePos;
	uint16			m_nBaseSize;
	BombTimerList	m_Timers;
	float			m_fScale;
};

#endif
