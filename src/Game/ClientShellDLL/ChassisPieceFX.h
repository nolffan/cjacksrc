// ----------------------------------------------------------------------- //
//
// MODULE  : ChassisPieceFX.h
//
// PURPOSE : The client's side of a ChassisPiece: its radar icon, and the
//           fade before it respawns.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x10017fc0..0x1001864a).  It is NOLF2's CDoomsdayPieceFX with the
// piece's type a ChassisPiece record of attributes\chassis.txt.
//
// ----------------------------------------------------------------------- //

#ifndef __CHASSISPIECE_FX_H__
#define __CHASSISPIECE_FX_H__

#include "SpecialFX.h"
#include "ClientFXMgr.h"

struct CHASSISPIECE;

struct CHASSISPIECECREATESTRUCT : public SFXCREATESTRUCT
{
	CHASSISPIECECREATESTRUCT();

	CHASSISPIECE const*	pChassisPiece;
	bool				bCarried;
	uint8				nTeam;
	bool				bPlanted;
	uint8				nActivateType;
	bool				bRespawnSoon;
};

inline CHASSISPIECECREATESTRUCT::CHASSISPIECECREATESTRUCT()
{
	pChassisPiece = LTNULL;
	bCarried = false;
	nTeam = INVALID_TEAM;
	bPlanted = false;
	nActivateType = 0;
	bRespawnSoon = false;
}

class ChassisPieceFX : public CSpecialFX
{
	public :

		~ChassisPieceFX();

		virtual LTBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual LTBOOL CreateObject(ILTClient* pClientDE);
		virtual LTBOOL Update();

		virtual uint32 GetSFXID() { return SFX_CHASSISPIECE_ID; }

		virtual LTBOOL OnServerMessage(ILTMessage_Read *pMsg);

		CHASSISPIECE const*	GetChassisPiece( ) const { return m_pChassisPiece; }
		bool		IsCarried() const { return m_bCarried; }
		uint8		GetTeam()  const { return m_nTeam; }
		bool		IsPlanted() const { return m_bPlanted; }
		uint8		GetActivateType() const { return m_nActivateType; }

	private :

		// The piece fades in and out while it waits to respawn.
		void		StartFlash();
		void		UpdateFlash();
		void		StopFlash();

		void		OnRespawn( LTVector const& vFrom, LTVector const& vTo );

		CLIENTFX_LINK		m_fxFlash;
		CHASSISPIECE const*	m_pChassisPiece;
		bool				m_bCarried;
		uint8				m_nTeam;
		bool				m_bPlanted;
		uint8				m_nActivateType;
		bool				m_bRespawnSoon;
		float				m_fAlpha;
		float				m_fAlphaDelta;
};

#endif // __CHASSISPIECE_FX_H__
