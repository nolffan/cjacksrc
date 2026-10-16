// ----------------------------------------------------------------------- //
//
// MODULE  : ChassisFX.h
//
// PURPOSE : The client's side of a Chassis: its team and drop zone.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x10017ed0..0x10017fb5).
//
// ----------------------------------------------------------------------- //

#ifndef __CHASSIS_FX_H__
#define __CHASSIS_FX_H__

#include "SpecialFX.h"

struct CHASSISCREATESTRUCT : public SFXCREATESTRUCT
{
	CHASSISCREATESTRUCT();

	uint8		nTeam;
	float		fDropZoneRadius;
	uint8		nActivateType;
};

inline CHASSISCREATESTRUCT::CHASSISCREATESTRUCT()
{
	nTeam = INVALID_TEAM;
	fDropZoneRadius = 0.0f;
	nActivateType = 0;
}

class ChassisFX : public CSpecialFX
{
	public :

		~ChassisFX();

		virtual LTBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
		virtual LTBOOL CreateObject(ILTClient* pClientDE);
		virtual LTBOOL Update();

		virtual uint32 GetSFXID() { return SFX_CHASSIS_ID; }

		virtual LTBOOL OnServerMessage(ILTMessage_Read *pMsg);

		uint8		GetTeam()				const { return m_nTeam; }
		float		GetDropZoneRadius()		const { return m_fDropZoneRadius; }
		uint8		GetActivateType()		const { return m_nActivateType; }

	private :

		uint8		m_nTeam;
		float		m_fDropZoneRadius;
		uint8		m_nActivateType;
};

#endif // __CHASSIS_FX_H__
