// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerVehicleFX.h
//
// PURPOSE : Tracer special fx class - Definition
//
// CREATED : 6/8/00
//
// (c) 2000 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __PLAYER_VEHICLE_FX_H__
#define __PLAYER_VEHICLE_FX_H__

#include "SpecialFX.h"
#include "SharedFXStructs.h"
#include "ClientFXMgr.h"

class CPlayerVehicleFX : public CSpecialFX
{
	public :

		CPlayerVehicleFX() : CSpecialFX()
		{
			m_bBlinking = false;
			m_bFadeIn = false;
			m_fAlpha = 1.0f;
		}

		// RETAIL: CShell 0x100a1400
		~CPlayerVehicleFX();

        virtual LTBOOL Init(HLOCALOBJ hServObj, ILTMessage_Read *pMsg);
        virtual LTBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
        virtual LTBOOL Update();

		virtual LTBOOL OnServerMessage(ILTMessage_Read *pMsg);

		virtual uint32 GetSFXID() { return SFX_PLAYERVEHICLE_ID; }

	protected :

		// RETAIL: Contract J.A.C.K.'s vehicles blink before they respawn
		// and move back to where they were placed.
		void			StartBlink();
		void			StopBlink();
		void			UpdateBlink();
		void			Move(LTVector const& vFrom, LTVector const& vTo);

		PVCREATESTRUCT	m_cs;
		bool			m_bBlinking;
		bool			m_bFadeIn;
		float			m_fAlpha;
		CLIENTFX_LINK	m_FadeFX;
};

#endif // __PLAYER_VEHICLE_FX_H__