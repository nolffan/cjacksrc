// ----------------------------------------------------------------------- //
//
// MODULE  : SnowVolume.h
//
// PURPOSE : Large-scale procedural snow volume declaration:
//           - snow specific parameters
//
// CREATED : 1/3/02
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
// ----------------------------------------------------------------------- //

#ifndef __SNOWVOLUME_H__
#define __SNOWVOLUME_H__

#include "VolumeEffect.h"


LINKTO_MODULE( SnowVolume );


class SnowVolume : public VolumeEffect
{
public:
	SnowVolume();
	~SnowVolume();

protected:
	float m_fDensity;
	float m_fParticleRadius;
	float m_fFallRate;
	float m_fTumbleRate;
	float m_fTumbleRadius;
	float m_fMaxDrawDist;
	LTVector m_vAmbientColor;
	bool m_bUseLighting;
	bool m_bUseSaturate;
	HSTRING m_hstrTextureName;
	bool m_bOn;			// RETAIL: Contract J.A.C.K.'s snow can be turned on and off

	uint32 EngineMessageFn( uint32 messageID, void* pData, LTFLOAT fData );
	bool OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg );

	// RETAIL: Object 0x10149070, 0x10149380
	virtual void Save( ILTMessage_Write *pMsg );
	virtual void Load( ILTMessage_Read *pMsg );

private:
	void WriteSnowInfo( ILTMessage_Write& cMsg );
	void UpdateSFX( bool bSendToClients );
	void TurnOn( bool bOn );
	bool ReadProp( ObjectCreateStruct* pOCS );
};


#endif // __SNOWVOLUME_H__