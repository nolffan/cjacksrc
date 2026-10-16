// ----------------------------------------------------------------------- //
//
// MODULE  : AimMagnetFX.cpp
//
// PURPOSE : AimMagnetFX - Implementation
//
// (c) 2003 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "AimMagnetFX.h"

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAimMagnetFX::Update
//
//	PURPOSE:	Update the AimMagnet
//
// ----------------------------------------------------------------------- //

// RETAIL: CShell 0x10001e70
LTBOOL CAimMagnetFX::Update()
{
    if (!m_pClientDE || m_bWantRemove || !m_hServerObject) return LTFALSE;

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAimMagnetFX::Init
//
//	PURPOSE:	Init the fx
//
// ----------------------------------------------------------------------- //

// RETAIL: CShell 0x10001e90
LTBOOL CAimMagnetFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
    if (!psfxCreateStruct) return LTFALSE;

	m_hServerObject = psfxCreateStruct->hServerObj;

	AIMMAGNETCREATESTRUCT* pAMCS = (AIMMAGNETCREATESTRUCT*)psfxCreateStruct;

	m_nTeamId = pAMCS->m_nTeamId;
	m_hTarget = pAMCS->m_hTarget;

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAimMagnetFX::CreateObject
//
//	PURPOSE:	Create object associated the fx
//
// ----------------------------------------------------------------------- //

// RETAIL: CShell 0x10001ec0
LTBOOL CAimMagnetFX::CreateObject(ILTClient *pClientDE)
{
    if (!pClientDE) return LTFALSE;

	m_pClientDE = pClientDE;

	if (m_hServerObject)
	{
		m_pClientDE->GetObjectPos(m_hServerObject, &m_vLastServPos);
	}

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAimMagnetFX::OnServerMessage
//
//	PURPOSE:	Handle recieving a message from the server...
//
// ----------------------------------------------------------------------- //

// RETAIL: CShell 0x10001ef0
LTBOOL CAimMagnetFX::OnServerMessage( ILTMessage_Read *pMsg )
{
	if( !pMsg ) return LTFALSE;

	m_nTeamId = pMsg->Readuint8( );
	m_hTarget = pMsg->ReadObject();

	return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAimMagnetFX::GetTarget
//
//	PURPOSE:	Get the handle of the target
//
// ----------------------------------------------------------------------- //

// RETAIL: CShell 0x10001f30
HOBJECT CAimMagnetFX::GetTarget() const
{
	if (m_hTarget)
		return m_hTarget;

	return GetServerObj();
}
