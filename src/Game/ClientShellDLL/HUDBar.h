// ----------------------------------------------------------------------- //
//
// MODULE  : HUDBar.h
//
// PURPOSE : Definition of "Bar" HUD component
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_BAR_H
#define __HUD_BAR_H

#include "ltbasedefs.h"
#include "LTPoly.h"

class CHUDBar
{
public:
	CHUDBar();

	void Init(HTEXTURE hBar);
	void Update(float x,float y, float fillW, float maxW, float h);
	void Render();

	// RETAIL: 0x10050d30 the bar's colour
	void SetColor(uint32 nColor);

private:
	bool		m_bInitted;

	LTPoly_GT4	m_Poly[6];
	HTEXTURE	m_Bar;

};

#endif