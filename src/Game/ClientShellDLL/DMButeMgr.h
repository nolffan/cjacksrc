// ----------------------------------------------------------------------- //
//
// MODULE  : DMButeMgr.h
//
// PURPOSE : The local bute manager that reads a multiplayer mission's
//           configuration (its world's .cfg).
//
// ----------------------------------------------------------------------- //

#ifndef __DMBUTEMGR_H__
#define __DMBUTEMGR_H__

#include "GameButeMgr.h"

// RETAIL: CScreenMulti's and CGameClientShell::CreateMPMissionFile's (its
// constructor CShell.dll 0x100350a0); its Init is in ScreenMulti.cpp.
class CDMButeMgr : public CGameButeMgr
{
	public :
        virtual LTBOOL	Init(const char* szAttributeFile);
		CButeMgr*	GetButeMgr() {return &m_buteMgr;}
};

#endif // __DMBUTEMGR_H__
