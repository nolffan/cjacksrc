// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerVehicleFX.cpp
//
// PURPOSE : Beam special FX - Implementation
//
// CREATED : 6/8/00
//
// (c) 2000 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "PlayerVehicleFX.h"
#include "iltclient.h"
#include "ClientUtilities.h"
#include "SFXMsgIds.h"
#include "GameClientShell.h"
#include "VehicleButeMgr.h"
#include "FxFlags.h"

#define RADAR_SNOWMOBILE_TYPE	"Snowmobile"

extern CGameClientShell* g_pGameClientShell;

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerVehicleFX::Init
//
//	PURPOSE:	Init the beam fx
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerVehicleFX::Init(HLOCALOBJ hServObj, ILTMessage_Read *pMsg)
{
    if (!CSpecialFX::Init(hServObj, pMsg)) return LTFALSE;
    if (!pMsg) return LTFALSE;

	PVCREATESTRUCT cs;

	cs.hServerObj = hServObj;
    cs.Read(pMsg);

	// RETAIL: CShell 0x100a16c0: whether the vehicle is about to respawn.
	m_bBlinking = pMsg->Readbool();
	if (m_bBlinking)
		StartBlink();
	else
		StopBlink();

	return Init(&cs);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerVehicleFX::Init
//
//	PURPOSE:	Init the player vehicle fx
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerVehicleFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
    if (!CSpecialFX::Init(psfxCreateStruct)) return LTFALSE;

	PVCREATESTRUCT* pFX = (PVCREATESTRUCT*)psfxCreateStruct;
	m_cs = *pFX;
	
	// Add the snowmobile object to the radar list in coop games only...

	if( IsMultiplayerGame( ) && IsCoopMultiplayerGameType( ) && g_pGameClientShell->ShouldUseRadar() )
	{
		g_pRadar->AddObject( m_hServerObject, RADAR_SNOWMOBILE_TYPE, INVALID_TEAM );
	}

     return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerVehicleFX::Update
//
//	PURPOSE:	Update the tracer
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerVehicleFX::Update()
{
	if( !CSpecialFX::Update() || m_bWantRemove ) 
		return LTFALSE;

	// RETAIL: CShell 0x100a14d0
	if (m_bBlinking)
		UpdateBlink();

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerVehicleFX::~CPlayerVehicleFX
//
//	PURPOSE:	RETAIL: CShell 0x100a1400
//
// ----------------------------------------------------------------------- //

CPlayerVehicleFX::~CPlayerVehicleFX()
{
	if (m_bBlinking)
	{
		m_bBlinking = false;
		g_pClientFXMgr->ShutdownClientFX(&m_FadeFX);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerVehicleFX::OnServerMessage
//
//	PURPOSE:	RETAIL: CShell 0x100a1750: the respawn messages
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerVehicleFX::OnServerMessage(ILTMessage_Read *pMsg)
{
	if (!pMsg) return LTFALSE;

	switch (pMsg->Readuint8())
	{
		case PVFX_BLINK:
			StartBlink();
		break;

		case PVFX_STOPBLINK:
			StopBlink();
		break;

		case PVFX_MOVE:
		{
			LTVector vFrom = pMsg->ReadLTVector();
			LTVector vTo = pMsg->ReadLTVector();
			Move(vFrom, vTo);
		}
		break;

		default:
		break;
	}

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerVehicleFX::StartBlink
//
//	PURPOSE:	RETAIL: CShell 0x100a1500: fade in and out until respawning
//
// ----------------------------------------------------------------------- //

void CPlayerVehicleFX::StartBlink()
{
	m_fAlpha = 1.0f;
	m_bFadeIn = false;
	m_bBlinking = true;

	VEHICLEBUTES* pVehicle = g_pVehicleButeMgr->GetVehicle(m_cs.nVehicleId);

	LTVector vPos;
	g_pLTClient->GetObjectPos(m_hServerObject, &vPos);

	if (pVehicle)
	{
		CLIENTFX_CREATESTRUCT fxInit(pVehicle->sMPFadeFX.c_str(), FXFLAG_LOOP | FXFLAG_NOSMOOTHSHUTDOWN, vPos);
		g_pClientFXMgr->CreateClientFX(&m_FadeFX, fxInit, LTTRUE);
	}

	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, 1.0f, 1.0f, 1.0f);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerVehicleFX::StopBlink
//
//	PURPOSE:	RETAIL: CShell 0x100a13b0
//
// ----------------------------------------------------------------------- //

void CPlayerVehicleFX::StopBlink()
{
	m_fAlpha = 1.0f;
	m_bBlinking = false;
	g_pClientFXMgr->ShutdownClientFX(&m_FadeFX);
	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, 1.0f, 1.0f, 1.0f);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerVehicleFX::UpdateBlink
//
//	PURPOSE:	RETAIL: CShell 0x100a1320
//
// ----------------------------------------------------------------------- //

// RETAIL: CShell .data 0x1014dbb0: the blink's rate, a variable.
static float s_fBlinkRate = 2.0f;

void CPlayerVehicleFX::UpdateBlink()
{
	if (m_bFadeIn)
	{
		m_fAlpha += g_pLTClient->GetFrameTime() * s_fBlinkRate;
		if (m_fAlpha > 1.0f)
		{
			m_fAlpha = 1.0f;
			m_bFadeIn = false;
		}
	}
	else
	{
		m_fAlpha -= g_pLTClient->GetFrameTime() * s_fBlinkRate;
		if (m_fAlpha < 0.01f)
		{
			m_fAlpha = 0.01f;
			m_bFadeIn = true;
		}
	}

	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, m_fAlpha, m_fAlpha, 1.0f);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerVehicleFX::Move
//
//	PURPOSE:	RETAIL: CShell 0x100a15b0: the vehicle went back to its
//				place
//
// ----------------------------------------------------------------------- //

void CPlayerVehicleFX::Move(LTVector const& vFrom, LTVector const& vTo)
{
	m_fAlpha = 1.0f;
	m_bBlinking = false;
	g_pClientFXMgr->ShutdownClientFX(&m_FadeFX);
	g_pLTClient->SetObjectColor(m_hServerObject, 1.0f, 1.0f, 1.0f, 1.0f);

	// RETAIL: 0x100a1606: no test of the vehicle, the positions as given.
	VEHICLEBUTES* pVehicle = g_pVehicleButeMgr->GetVehicle(m_cs.nVehicleId);

	CLIENTFX_CREATESTRUCT fxUnspawn(pVehicle->sMPUnspawnFX.c_str(), 0, const_cast<LTVector&>(vFrom));
	CLIENTFX_LINK fxUnspawnLink;
	g_pClientFXMgr->CreateClientFX(&fxUnspawnLink, fxUnspawn, LTTRUE);

	CLIENTFX_CREATESTRUCT fxSpawn(pVehicle->sMPSpawnFX.c_str(), 0, const_cast<LTVector&>(vTo));
	CLIENTFX_LINK fxSpawnLink;
	g_pClientFXMgr->CreateClientFX(&fxSpawnLink, fxSpawn, LTTRUE);
}