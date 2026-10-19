// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenHostDemolitionOptions.h
//
// PURPOSE : Interface screen for hosting Demolition games
//
// (c) 1999-2003 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

// RETAIL: Contract J.A.C.K.'s own (CShell.dll 0x100c40d0..0x100c46a1, its
// vtable 0x101369f4); written from the retail code, modelled on the other
// host options screens.

#ifndef _SCREEN_HOST_DEMOLITION_OPTIONS_H_
#define _SCREEN_HOST_DEMOLITION_OPTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseScreen.h"
#include "NetDefs.h"

class CScreenHostDemolitionOptions : public CBaseScreen
{
public:
	CScreenHostDemolitionOptions();
	virtual ~CScreenHostDemolitionOptions();

	// Build the screen
    LTBOOL   Build();

    void    OnFocus(LTBOOL bFocus);

protected:
    uint32  OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);

	int			m_nMaxPlayers;
	int			m_nRunSpeed;
	int			m_nRounds;
	LTBOOL		m_bFriendlyFire;
	int			m_nFragScore;
	LTBOOL		m_bWeaponsStay;

	CLTGUISlider*	m_pMaxPlayers;
	CLTGUITextCtrl*	m_pTeam1; 
	CLTGUITextCtrl*	m_pTeam2; 
	// RETAIL: still there, unused (the screen is 0x7b8 bytes)
	CLTGUITextCtrl*	m_pTeam1Name; 
	CLTGUITextCtrl*	m_pTeam2Name; 
};

#endif // _SCREEN_HOST_DEMOLITION_OPTIONS_H_
