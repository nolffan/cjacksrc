// ----------------------------------------------------------------------- //
//
// MODULE  : DoomsDayPiece.cpp
//
// PURPOSE : The object used for collecting DoomsDay pieces 
//
// CREATED : 12/13/02
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

//
// Includes...
//

	#include "stdafx.h"
	#include "ParsedMsg.h"
	#include "PlayerObj.h"
	#include "DoomsDayDevice.h"
	#include "DoomsDayPiece.h"
	#include "ChassisButeMgr.h"
	#include "ServerButeMgr.h"

//
// Globals...
//
	
// The DoomsDay pieces' chassis pieces in attributes\chassis.txt.
char const* c_aszDDPieceTypes[] =
{
	"Doomsday_transmitter",
	"Doomsday_batteries",
	"Doomsday_Core",
};

LINKFROM_MODULE( DoomsDayPiece );

BEGIN_CLASS( DoomsDayPiece )

	ADD_STRINGPROP_FLAG(Type, "", PF_STATICLIST | PF_DIMS | PF_LOCALDIMS)
	ADD_STRINGPROP_FLAG(ChassisPieceType, "", PF_HIDDEN)
	ADD_REALPROP_FLAG(RespawnTime, 0.0f, PF_HIDDEN)

END_CLASS_DEFAULT_FLAGS_PLUGIN( DoomsDayPiece, ChassisPiece, NULL, NULL, 0, CDoomsDayPiecePlugin )


// Register with the CommandMgr...

CMDMGR_BEGIN_REGISTER_CLASS( DoomsDayPiece )
CMDMGR_END_REGISTER_CLASS( DoomsDayPiece, ChassisPiece )



LTRESULT CDoomsDayPiecePlugin::PreHook_EditStringList(
	const char *szRezPath,
	const char *szPropName,
	char **aszStrings,
	uint32 *pcStrings,
	const uint32 cMaxStrings,
	const uint32 cMaxStringLen )
{
	if( _strcmpi("Type", szPropName) == 0 )
	{
		// Fill the list with our piece types...

		for( uint32 i = 0; i < kDoomsDay_MAXTYPES; i++ )	// RETAIL: Object 0x100c8ab0: unsigned
		{
			strcpy( aszStrings[(*pcStrings)++], c_aszDDPieceTypes[i] );
		}
		
		return LT_OK;
	}
	else if( LT_OK == CPropTypePlugin::PreHook_EditStringList( szRezPath, szPropName, aszStrings, pcStrings, cMaxStrings, cMaxStringLen ))
	{
		return LT_OK;
	}

	return LT_UNSUPPORTED;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	DoomsDayPiece::DoomsDayPiece
//
//  PURPOSE:	Constructor...
//
// ----------------------------------------------------------------------- //

DoomsDayPiece::DoomsDayPiece( )
:	ChassisPiece	( )
{
	
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	DoomsDayPiece::DoomsDayPiece
//
//  PURPOSE:	Destructor...
//
// ----------------------------------------------------------------------- //

DoomsDayPiece::~DoomsDayPiece( )
{

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	DoomsDayPiece::EngineMessageFn
//
//	PURPOSE:	Handle engine messages
//
// ----------------------------------------------------------------------- //

uint32 DoomsDayPiece::EngineMessageFn(uint32 messageID, void *pData, LTFLOAT fData)
{
	switch(messageID)
	{
		case MID_PRECREATE:
		{
			if( fData == PRECREATE_WORLDFILE || fData == PRECREATE_STRINGPROP )
			{
				ReadProp( (ObjectCreateStruct*)pData );
			}
		}
		break;
		
		case MID_INITIALUPDATE:
		{
			uint32 dwRet = ChassisPiece::EngineMessageFn(messageID, pData, fData);

			// A dropped piece goes back after the DoomsDay respawn time.

			m_fRespawnTime = g_pServerButeMgr->GetDoomsDayDeviceRespawnTime();

			return dwRet;
		}
		break;

		default : break;
	}

	return ChassisPiece::EngineMessageFn(messageID, pData, fData);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	DoomsDayPiece::PropRead()
//
//	PURPOSE:	Update properties
//
// ----------------------------------------------------------------------- //

bool DoomsDayPiece::ReadProp( ObjectCreateStruct *pOCS )
{
	if( !pOCS )
		return false;

	GenericProp GenProp;

	if( g_pLTServer->GetPropGeneric( "Type", &GenProp ) == LT_OK )
	{
		if( GenProp.m_String[0] )
		{
			m_pChassisPiece = ChassisButeMgr::Instance( ).GetPiece( GenProp.m_String );
		}
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	DoomsDayPiece::OnCarry
//
//	PURPOSE:	Handle the Carry message...
//
// ----------------------------------------------------------------------- //

bool DoomsDayPiece::OnCarry( HOBJECT hSender )
{
	CPlayerObj *pPlayer = dynamic_cast< CPlayerObj* >(g_pLTServer->HandleToObject( hSender ));
	if( !pPlayer )
		return false;

	uint8 nDDAction = MID_DOOMSDAY_PIECE_PICKEDUP;
	
	DoomsDayDevice *pDevice = dynamic_cast< DoomsDayDevice* >(g_pLTServer->HandleToObject( m_hChassis ));
	if( pDevice )
	{
		// Don't allow the player to remove pieces from it's own device....

		if( pDevice->GetTeamID() != INVALID_TEAM && pPlayer->GetTeamID() == pDevice->GetTeamID() )
			return false;

		nDDAction = MID_DOOMSDAY_PIECE_STOLEN;
	}

	if( !ChassisPiece::OnCarry( hSender ))
		return false;

	// Send a message letting the players know which team currently has the piece...

	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_DOOMSDAY_MESSAGE );
	cMsg.Writeuint8( nDDAction );
	cMsg.Writeuint8( m_pChassisPiece->m_nId );
	cMsg.Writeuint8( pPlayer->GetTeamID() );
	g_pLTServer->SendToClient( cMsg.Read(), LTNULL, MESSAGE_GUARANTEED );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	DoomsDayPiece::OnDrop
//
//	PURPOSE:	Handle the Drop message...
//
// ----------------------------------------------------------------------- //

bool DoomsDayPiece::OnDrop( HOBJECT hSender )
{
	CPlayerObj *pPlayer = dynamic_cast< CPlayerObj* >(g_pLTServer->HandleToObject( hSender ));
	if( !pPlayer )
		return false;

	if( !ChassisPiece::OnDrop( hSender ))
		return false;

	uint8 nDDAction = MID_DOOMSDAY_PIECE_DROPPED;
	bool bComplete = false;

	if( m_hChassis )
	{
		// Placed on a device; the device says itself when it is complete...

		DoomsDayDevice *pDevice = dynamic_cast< DoomsDayDevice* >(g_pLTServer->HandleToObject( m_hChassis ));
		if( pDevice )
			bComplete = pDevice->IsComplete();

		nDDAction = MID_DOOMSDAY_PIECE_PLACED;

		if( bComplete )
			return true;
	}

	// Send a message letting the players know which team currently has the piece...

	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_DOOMSDAY_MESSAGE );
	cMsg.Writeuint8( nDDAction );
	cMsg.Writeuint8( m_pChassisPiece->m_nId );
	cMsg.Writeuint8( pPlayer->GetTeamID() );
	g_pLTServer->SendToClient( cMsg.Read(), LTNULL, MESSAGE_GUARANTEED );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	DoomsDayPiece::Respawn
//
//	PURPOSE:	Place the piece back at it's original position...
//
// ----------------------------------------------------------------------- //

bool DoomsDayPiece::Respawn( )
{
	if( !ChassisPiece::Respawn( ))
		return false;

	// Let the clients know it respawned...

	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_DOOMSDAY_MESSAGE );
	cMsg.Writeuint8( MID_DOOMSDAY_PIECE_RESPAWNED );
	cMsg.Writeuint8( m_pChassisPiece->m_nId );
	g_pLTServer->SendToClient( cMsg.Read(), LTNULL, MESSAGE_GUARANTEED );

	return true;
}
