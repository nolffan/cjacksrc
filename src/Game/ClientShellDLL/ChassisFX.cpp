// ----------------------------------------------------------------------- //
//
// MODULE  : ChassisFX.cpp
//
// PURPOSE : The client's side of a Chassis.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x10017ed0..0x10017fb5).
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "ChassisFX.h"
#include "iltclient.h"
#include "ClientServerShared.h"
#include "SFXMsgIds.h"

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisFX::Update
//
// ----------------------------------------------------------------------- //

LTBOOL ChassisFX::Update()
{
	if (!m_pClientDE || m_bWantRemove || !m_hServerObject) return LTFALSE;

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisFX::Init
//
// ----------------------------------------------------------------------- //

LTBOOL ChassisFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CSpecialFX::Init(psfxCreateStruct)) return LTFALSE;

	CHASSISCREATESTRUCT* pCS = (CHASSISCREATESTRUCT*)psfxCreateStruct;

	m_nTeam				= pCS->nTeam;
	m_fDropZoneRadius	= pCS->fDropZoneRadius;
	m_nActivateType		= pCS->nActivateType;

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisFX::CreateObject
//
// ----------------------------------------------------------------------- //

LTBOOL ChassisFX::CreateObject(ILTClient *pClientDE)
{
	LTBOOL bRet = CSpecialFX::CreateObject(pClientDE);
	if (!bRet) return bRet;

	return bRet;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisFX::OnServerMessage
//
// ----------------------------------------------------------------------- //

LTBOOL ChassisFX::OnServerMessage(ILTMessage_Read *pMsg)
{
	if (!CSpecialFX::OnServerMessage(pMsg)) return LTFALSE;

	m_nTeam = pMsg->Readuint8();
	m_fDropZoneRadius = pMsg->Readfloat();

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisFX::~ChassisFX
//
// ----------------------------------------------------------------------- //

ChassisFX::~ChassisFX()
{
}
