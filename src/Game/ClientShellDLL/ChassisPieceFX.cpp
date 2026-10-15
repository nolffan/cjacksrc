// ----------------------------------------------------------------------- //
//
// MODULE  : ChassisPieceFX.cpp
//
// PURPOSE : The client's side of a ChassisPiece.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x10017fc0..0x1001864a), starting from NOLF2's DoomsdayPieceFX.cpp.
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "ChassisPieceFX.h"
#include "ChassisButeMgr.h"
#include "iltclient.h"
#include "ClientUtilities.h"
#include "ClientServerShared.h"
#include "GameClientShell.h"
#include "SFXMsgIds.h"
#include "HUDMgr.h"

extern CGameClientShell* g_pGameClientShell;

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPieceFX::UpdateFlash
//
//	PURPOSE:	Fade the piece out and in.
//
// ----------------------------------------------------------------------- //

void ChassisPieceFX::UpdateFlash()
{
	m_fAlpha += g_pLTClient->GetFrameTime() * m_fAlphaDelta;

	if (m_fAlpha > 1.0f)
	{
		m_fAlpha = 1.0f;
		m_fAlphaDelta = -1.0f;
	}
	else if (m_fAlpha < 0.1f)
	{
		m_fAlpha = 0.1f;
		m_fAlphaDelta = 1.0f;
	}

	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, 1.0f, m_fAlpha, m_fAlpha);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPieceFX::StopFlash
//
// ----------------------------------------------------------------------- //

void ChassisPieceFX::StopFlash()
{
	m_fAlpha = 1.0f;
	m_bRespawnSoon = false;
	m_fAlphaDelta = -1.0f;

	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, 1.0f, 1.0f, 1.0f);

	g_pClientFXMgr->ShutdownClientFX(&m_fxFlash);

	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, 1.0f, 1.0f, 1.0f);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPieceFX::Init
//
//	PURPOSE:	Init the fx
//
// ----------------------------------------------------------------------- //

LTBOOL ChassisPieceFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CSpecialFX::Init(psfxCreateStruct)) return LTFALSE;

	CHASSISPIECECREATESTRUCT* pCS = (CHASSISPIECECREATESTRUCT*)psfxCreateStruct;

	m_pChassisPiece	= pCS->pChassisPiece;
	m_bCarried		= pCS->bCarried;
	m_nTeam			= pCS->nTeam;
	m_bPlanted		= pCS->bPlanted;
	m_nActivateType	= pCS->nActivateType;
	m_bRespawnSoon	= pCS->bRespawnSoon;
	m_fAlpha		= 1.0f;
	m_fAlphaDelta	= -1.0f;

	std::string sRadarType = m_pChassisPiece->m_sRadarType;

	switch (m_nTeam)
	{
	case 0:
		sRadarType += "_B";
		break;
	case 1:
		sRadarType += "_R";
		break;
	default:
		sRadarType += "_N";
		break;
	}
	g_pRadar->AddObject(m_hServerObject, sRadarType.c_str(), INVALID_TEAM);

	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, 1.0f, 1.0f, m_fAlpha);

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPieceFX::CreateObject
//
//	PURPOSE:	Create object associated the fx
//
// ----------------------------------------------------------------------- //

LTBOOL ChassisPieceFX::CreateObject(ILTClient *pClientDE)
{
	LTBOOL bRet = CSpecialFX::CreateObject(pClientDE);
	if (!bRet) return bRet;

	return bRet;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPieceFX::Update
//
//	PURPOSE:	Update the piece
//
// ----------------------------------------------------------------------- //

LTBOOL ChassisPieceFX::Update()
{
	if (!m_pClientDE || m_bWantRemove || !m_hServerObject) return LTFALSE;

	if (m_bRespawnSoon)
	{
		UpdateFlash();
	}

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPieceFX::~ChassisPieceFX
//
// ----------------------------------------------------------------------- //

ChassisPieceFX::~ChassisPieceFX()
{
	StopFlash();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPieceFX::StartFlash
//
//	PURPOSE:	Start fading the piece, with its fade fx.
//
// ----------------------------------------------------------------------- //

void ChassisPieceFX::StartFlash()
{
	m_fAlpha = 1.0f;
	m_bRespawnSoon = true;
	m_fAlphaDelta = -1.0f;

	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, 1.0f, 1.0f, 1.0f);

	LTVector vPos;
	g_pLTClient->GetObjectPos(m_hServerObject, &vPos);

	CLIENTFX_CREATESTRUCT fxInit( m_pChassisPiece->m_sFadeFX.c_str(), FXFLAG_LOOP | FXFLAG_NOSMOOTHSHUTDOWN, vPos );
	g_pClientFXMgr->CreateClientFX(&m_fxFlash, fxInit, LTTRUE);

	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, 1.0f, 1.0f, 1.0f);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPieceFX::OnRespawn
//
//	PURPOSE:	The piece went back where it started.
//
// ----------------------------------------------------------------------- //

void ChassisPieceFX::OnRespawn( LTVector const& vFrom, LTVector const& vTo )
{
	StopFlash();

	CLIENTFX_CREATESTRUCT fxUnspawnInit( m_pChassisPiece->m_sUnspawnFX.c_str(), 0, (LTVector&)vFrom );
	CLIENTFX_LINK fxUnspawn;
	g_pClientFXMgr->CreateClientFX(&fxUnspawn, fxUnspawnInit, LTTRUE);

	CLIENTFX_CREATESTRUCT fxSpawnInit( m_pChassisPiece->m_sSpawnFX.c_str(), 0, (LTVector&)vTo );
	CLIENTFX_LINK fxSpawn;
	g_pClientFXMgr->CreateClientFX(&fxSpawn, fxSpawnInit, LTTRUE);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPieceFX::OnServerMessage()
//
//	PURPOSE:	We got a message!
//
// ----------------------------------------------------------------------- //

LTBOOL ChassisPieceFX::OnServerMessage(ILTMessage_Read *pMsg)
{
	if (!CSpecialFX::OnServerMessage(pMsg)) return LTFALSE;

	if (pMsg->Readbool())
	{
		// The piece respawned...

		LTVector vFrom = pMsg->ReadLTVector();
		LTVector vTo = pMsg->ReadLTVector();

		m_bCarried = false;
		m_nTeam = INVALID_TEAM;
		m_bPlanted = false;

		OnRespawn( vFrom, vTo );
	}
	else
	{
		m_pChassisPiece = ChassisButeMgr::Instance( ).GetPiece( pMsg->Readuint8() );
		m_bCarried = pMsg->Readbool();
		m_nTeam = pMsg->Readuint8();
		m_bPlanted = pMsg->Readbool();
		pMsg->Readuint8();

		bool bRespawnSoon = pMsg->Readbool();
		if (bRespawnSoon != m_bRespawnSoon)
		{
			if (bRespawnSoon)
				StartFlash();
			else
				StopFlash();
		}
	}

	std::string sRadarType = m_pChassisPiece->m_sRadarType;

	switch (m_nTeam)
	{
	case 0:
		sRadarType += "_B";
		break;
	case 1:
		sRadarType += "_R";
		break;
	default:
		sRadarType += "_N";
		break;
	}
	g_pRadar->ChangeRadarType(m_hServerObject, sRadarType.c_str());

	g_pHUDMgr->QueueUpdate(kHUDDoomsday);

	return LTTRUE;
}
