// ----------------------------------------------------------------------- //
//
// MODULE  : HUDWeaponHeat.h
//
// PURPOSE : HUDItem to display the current weapon's heat
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x100621e0..0x100625ae).
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_WEAPONHEAT_H
#define __HUD_WEAPONHEAT_H

#include "HUDItem.h"
#include "HUDBar.h"

class CHUDWeaponHeat : public CHUDItem
{
public:
	CHUDWeaponHeat();

	LTBOOL		Init();
	void		Term();

	void		Render();
	void		Update();

	void		UpdateLayout();

private:

	LTIntPt		m_BasePos;
	int			m_nBarHeight;
	float		m_fBarScale;

	CHUDBar		m_Bar;
	bool		m_bDraw;

	// The bar goes from cool to warm to hot as the weapon heats up, and
	// shows the overheat colour once it is too hot.
	LTVector	m_vCool;
	LTVector	m_vWarm;
	LTVector	m_vHot;
	uint32		m_nOverheatColor;
};

#endif
