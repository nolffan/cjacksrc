// ----------------------------------------------------------------------- //
//
// MODULE  : AimMagnetFX.h
//
// PURPOSE : AimMagnetFX - Definition: a point the auto targeting aims at
//
// (c) 2003 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

// RETAIL: Contract J.A.C.K.'s (CShell.dll 0x10001e70..0x10001f3a, its
// vtable 0x10137eb8; created from SFX_AIMMAGNET_ID by the server's
// AimMagnet objects). Written from the retail code; FEAR's
// (ClientShellDLL/AimMagnetFX) is its descendant.

#ifndef __AIMMAGNET_FX_H__
#define __AIMMAGNET_FX_H__

#include "SpecialFX.h"
#include "SFXMsgIds.h"
#include "TeamMgr.h"

struct AIMMAGNETCREATESTRUCT : public SFXCREATESTRUCT
{
    AIMMAGNETCREATESTRUCT();

	uint8		m_nTeamId;
	HOBJECT		m_hTarget;
};

inline AIMMAGNETCREATESTRUCT::AIMMAGNETCREATESTRUCT()
{
	m_nTeamId = INVALID_TEAM;
	m_hTarget = LTNULL;
}

class CAimMagnetFX : public CSpecialFX
{
	public :

        virtual LTBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
        virtual LTBOOL CreateObject(ILTClient* pClientDE);
        virtual LTBOOL Update();

		virtual LTBOOL OnServerMessage( ILTMessage_Read *pMsg ); 

		virtual uint32 GetSFXID() { return SFX_AIMMAGNET_ID; }

		uint8	GetTeamId( ) const { return m_nTeamId; }

		//get the target object if there is one, otherwise get the server obj
		HOBJECT	GetTarget() const;

	private :

		uint8	m_nTeamId;
		HOBJECT m_hTarget;
};

#endif // __AIMMAGNET_FX_H__
