// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenMulti.h
//
// PURPOSE : Interface screen for hosting and joining multi player games
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

// RETAIL: Contract J.A.C.K.'s multiplayer screen (CShell.dll
// 0x100d3130..0x100d3cbb, its vtable 0x101373dc): Player setup, CD Key,
// Join, Host. NOLF2's without the WON server directory, its version check
// and its messages of the day; the CD key is checked by the GameSpy browser.

#ifndef _SCREEN_MULTI_H_
#define _SCREEN_MULTI_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseScreen.h"

class CScreenMulti : public CBaseScreen
{
public:
	CScreenMulti();
	virtual ~CScreenMulti();

	// Build the screen
    LTBOOL   Build();
	LTBOOL	Render(HSURFACE hDestSurf);
    void    OnFocus(LTBOOL bFocus);
	void	Escape();

protected:
    uint32	OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
	uint32	HandleCallback(uint32 dwParam1, uint32 dwParam2);

	void	ChangeCDKey();
	void	Update();

	CLTGUIColumnCtrl* m_pCDKeyCtrl;
	std::string m_sCurCDKey;
	std::string m_sLastValidCDKey;

	enum EState {
		eState_Startup,
		eState_VersionCheck,
		eState_NoCDKey,
		eState_ValidateCDKey,
		eState_Ready,
	};

	EState m_eCurState;

	CLTGUIWindow*	m_pWait;
	CLTGUICtrl*		m_pJoin;
	CLTGUICtrl*		m_pHost;
	CLTGUITextCtrl*		m_pStatusCtrl;
	CLTGUITextCtrl*		m_pUpdate;
	CLTGUITextCtrl*		m_pWaitText;
};

#endif // _SCREEN_MULTI_H_
