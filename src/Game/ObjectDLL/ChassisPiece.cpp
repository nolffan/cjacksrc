// ----------------------------------------------------------------------- //
//
// MODULE  : ChassisPiece.cpp
//
// PURPOSE : A piece a player carries to a Chassis and puts on it.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from Object.lto
// (0x100ae720..0x100af950), starting from NOLF2's DoomsDayPiece.
//
// ----------------------------------------------------------------------- //

//
// Includes...
//

	#include "stdafx.h"
	#include "ParsedMsg.h"
	#include "PlayerObj.h"
	#include "Chassis.h"
	#include "ChassisPiece.h"
	#include "ChassisButeMgr.h"
	#include "CommonUtilities.h"

LINKFROM_MODULE( ChassisPiece );

BEGIN_CLASS( ChassisPiece )

	ADD_STRINGPROP_FLAG(Type, "", PF_HIDDEN)
	ADD_STRINGPROP_FLAG(ChassisPieceType, "", PF_STATICLIST | PF_DIMS | PF_LOCALDIMS)
	ADD_REALPROP_FLAG(RespawnTime, 0.0f, 0)

END_CLASS_DEFAULT_FLAGS_PLUGIN( ChassisPiece, PropType, NULL, NULL, 0, CChassisPiecePlugin )


// Register with the CommandMgr...

CMDMGR_BEGIN_REGISTER_CLASS( ChassisPiece )
CMDMGR_END_REGISTER_CLASS( ChassisPiece, PropType )


LTRESULT CChassisPiecePlugin::PreHook_EditStringList(
	const char *szRezPath,
	const char *szPropName,
	char **aszStrings,
	uint32 *pcStrings,
	const uint32 cMaxStrings,
	const uint32 cMaxStringLen )
{
	if( _strcmpi( "ChassisPieceType", szPropName ) == 0 )
	{
		if( !ChassisButeMgr::Instance( ).IsInitialized( ))
		{
			std::string sFile = szRezPath;
			sFile += "\\";
			sFile += CHASSISBMGR_DEFAULT_FILE;

			ChassisButeMgr::Instance( ).SetInRezFile( LTFALSE );
			ChassisButeMgr::Instance( ).Init( sFile.c_str( ));
		}

		// Fill the list with the chassis pieces...

		ChassisButeMgr::PieceList::const_iterator iter = ChassisButeMgr::Instance( ).GetPieceList( ).begin( );
		for( ; iter != ChassisButeMgr::Instance( ).GetPieceList( ).end( ); ++iter )
		{
			if( cMaxStrings > (*pcStrings) + 1 && *iter && !(*iter)->m_sName.empty( ))
			{
				LTStrCpy( aszStrings[(*pcStrings)++], (*iter)->m_sName.c_str( ), cMaxStringLen );
			}
		}

		qsort( aszStrings, *pcStrings, sizeof( char * ), CaseInsensitiveCompare );

		return LT_OK;
	}
	else if( LT_OK == CPropTypePlugin::PreHook_EditStringList( szRezPath, szPropName, aszStrings, pcStrings, cMaxStrings, cMaxStringLen ))
	{
		return LT_OK;
	}

	return LT_UNSUPPORTED;
}

LTRESULT CChassisPiecePlugin::PreHook_Dims(
	const char *szRezPath,
	const char *szPropValue,
	char *szModelFilenameBuf,
	int nModelFilenameBufLen,
	LTVector &vDims )
{
	if( !ChassisButeMgr::Instance( ).IsInitialized( ))
	{
		std::string sFile = szRezPath;
		sFile += "\\";
		sFile += CHASSISBMGR_DEFAULT_FILE;

		ChassisButeMgr::Instance( ).SetInRezFile( LTFALSE );
		ChassisButeMgr::Instance( ).Init( sFile.c_str( ));
	}

	CHASSISPIECE const* pType = ChassisButeMgr::Instance( ).GetPiece( szPropValue );
	if( pType )
	{
		// RETAIL: 0x100ae91d this gives WorldEdit the piece's PropType name,
		// not the PropType's model.
		strcpy( szModelFilenameBuf, pType->m_sPropType.c_str( ));
		ConvertLTBFilename( szModelFilenameBuf );

		return LT_OK;
	}

	if( LT_OK == CPropTypePlugin::PreHook_Dims( szRezPath, szPropValue, szModelFilenameBuf, nModelFilenameBufLen, vDims ))
	{
		return LT_OK;
	}

	return LT_UNSUPPORTED;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	ChassisPiece::ChassisPiece
//
//  PURPOSE:	Constructor...
//
// ----------------------------------------------------------------------- //

ChassisPiece::ChassisPiece( )
:	PropType		( ),
	m_hChassis		( LTNULL ),
	m_fRespawnTime	( 0.0f ),
	m_bRespawnSoon	( false ),
	m_vOriginalPos	( 0.0f, 0.0f, 0.0f )
{
	m_pChassisPiece = LTNULL;
	m_bCanBeCarried = true;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	ChassisPiece::~ChassisPiece
//
//  PURPOSE:	Destructor...
//
// ----------------------------------------------------------------------- //

ChassisPiece::~ChassisPiece( )
{

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::EngineMessageFn
//
//	PURPOSE:	Handle engine messages
//
// ----------------------------------------------------------------------- //

uint32 ChassisPiece::EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData )
{
	switch( messageID )
	{
		case MID_PRECREATE:
		{
			if( fData == PRECREATE_WORLDFILE || fData == PRECREATE_STRINGPROP )
			{
				ReadProp(( ObjectCreateStruct* )pData );
			}
		}
		break;

		case MID_INITIALUPDATE:
		{
			uint32 dwRet = PropType::EngineMessageFn( messageID, pData, fData );

			// Cache the position to be used for respawning the piece when dropped...

			g_pLTServer->GetObjectPos( m_hObject, &m_vOriginalPos );

			CreateSFXMessage( false, INVALID_TEAM, true );

			return dwRet;
		}
		break;

		case MID_UPDATE:
		{
			uint32 dwRet = PropType::EngineMessageFn( messageID, pData, fData );

			Update( );

			return dwRet;
		}
		break;

		case MID_SAVEOBJECT:
		{
			Save(( ILTMessage_Write* )pData, ( uint32 )fData );
		}
		break;

		case MID_LOADOBJECT:
		{
			Load(( ILTMessage_Read* )pData, ( uint32 )fData );
		}
		break;

		default : break;
	}

	return PropType::EngineMessageFn( messageID, pData, fData );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::SetNextUpdate
//
//	PURPOSE:	A piece left without updates may be deactivated.
//
// ----------------------------------------------------------------------- //

void ChassisPiece::SetNextUpdate( LTFLOAT fDelta, eUpdateControl eControl )
{
	m_bCanDeactivate = ( fDelta == UPDATE_NEVER && eControl == eControlDeactivation );

	PropType::SetNextUpdate( fDelta, eControl );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::ReadProp
//
//	PURPOSE:	Update properties
//
// ----------------------------------------------------------------------- //

bool ChassisPiece::ReadProp( ObjectCreateStruct *pOCS )
{
	if( !pOCS )
		return false;

	GenericProp GenProp;

	if( g_pLTServer->GetPropGeneric( "ChassisPieceType", &GenProp ) == LT_OK && GenProp.m_String[0] )
	{
		m_pChassisPiece = ChassisButeMgr::Instance( ).GetPiece( GenProp.m_String );
		if( m_pChassisPiece )
		{
			m_sPropType = m_pChassisPiece->m_sPropType.c_str( );
		}
	}

	if( g_pLTServer->GetPropGeneric( "RespawnTime", &GenProp ) == LT_OK )
	{
		m_fRespawnTime = GenProp.m_Float;
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::OnTrigger
//
//	PURPOSE:	Handle trigger messages
//
// ----------------------------------------------------------------------- //

bool ChassisPiece::OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg )
{
	static CParsedMsg::CToken	s_cTok_Carry( "CARRY" );
	static CParsedMsg::CToken	s_cTok_Drop( "DROP" );

	if( cMsg.GetArg( 0 ) == s_cTok_Carry )
	{
		if( !OnCarry( hSender ))
			return false;

		m_bCanBeCarried = false;
	}
	else if( cMsg.GetArg( 0 ) == s_cTok_Drop )
	{
		if( !OnDrop( hSender ))
			return false;

		m_bCanBeCarried = true;
	}

	return PropType::OnTrigger( hSender, cMsg );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::IsHeavy
//
//	PURPOSE:	Checks if piece is heavy type.
//
// ----------------------------------------------------------------------- //

bool ChassisPiece::IsHeavy( ) const
{
	return m_pChassisPiece ? m_pChassisPiece->m_bHeavy : false;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::OnCarry
//
//	PURPOSE:	Handle the Carry message...
//
// ----------------------------------------------------------------------- //

bool ChassisPiece::OnCarry( HOBJECT hSender )
{
	if( !m_bCanBeCarried )
		return false;

	CPlayerObj *pPlayer = dynamic_cast< CPlayerObj* >( g_pLTServer->HandleToObject( hSender ));
	if( !pPlayer )
		return false;

	if( !m_DelayTimer.Stopped() )
		return false;

	// Removing the piece from a chassis...

	Chassis *pChassis = dynamic_cast< Chassis* >( g_pLTServer->HandleToObject( m_hChassis ));
	if( pChassis )
	{
		if( !pChassis->RemovePiece( this, pPlayer ))
			return false;

		m_hChassis = LTNULL;
	}

	// Go non-solid so the character can move while carrying it...

	g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_SOLID | FLAG_RAYHIT );
	g_pCommonLT->SetObjectFlags( m_hObject, OFT_User, USRFLG_ATTACH_HIDE1SHOW3, USRFLG_ATTACH_HIDE1SHOW3 );

	ShowObjectAttachments( m_hObject, false );

	// Set this object as the players carried object...

	pPlayer->SetCarriedObject( m_hObject );

	// Stop the respawn counter since the piece is being carried again...

	m_bRespawnSoon = false;
	m_RespawnTimer.Stop();
	m_DelayTimer.Stop();
	SetNextUpdate( UPDATE_NEVER );

	CreateSFXMessage( true, pPlayer->GetTeamID( ), true );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::OnDrop
//
//	PURPOSE:	Handle the Drop message...
//
// ----------------------------------------------------------------------- //

bool ChassisPiece::OnDrop( HOBJECT hSender )
{
	// Make sure the piece can be picked up again...

	g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, FLAG_RAYHIT , FLAG_RAYHIT );

	// See if we are being dropped near a Chassis...

	CPlayerObj *pPlayer = dynamic_cast< CPlayerObj* >( g_pLTServer->HandleToObject( hSender ));
	if( !pPlayer )
		return false;

	LTVector vDropPos;
	g_pLTServer->GetObjectPos( hSender, &vDropPos );

	LTVector vChassisPos;

	Chassis::ChassisList::const_iterator iter = Chassis::GetChassisList().begin();
	while( iter != Chassis::GetChassisList().end() )
	{
		Chassis *pChassis = *iter;
		g_pLTServer->GetObjectPos( pChassis->m_hObject, &vChassisPos );

		if( ( vDropPos.Dist( vChassisPos ) <= pChassis->GetDropZoneRadius() ) &&
			( pChassis->GetTeamID() == INVALID_TEAM || pChassis->GetTeamID() == pPlayer->GetTeamID() ))
		{
			// Dropping close to a chassis...

			if( pChassis->AddPiece( this, pPlayer ) )
			{
				// Let the piece know it's now part of a chassis...

				m_hChassis = pChassis->m_hObject;

				CreateSFXMessage( false, pPlayer->GetTeamID(), true );

				return true;
			}
		}

		++iter;
	}

	// Begin our respawn timer...

	if( m_fRespawnTime > 0.0f )
	{
		m_RespawnTimer.Start( m_fRespawnTime );
	}

	m_bRespawnSoon = false;
	CreateSFXMessage( false, INVALID_TEAM, true );

	// Not close enough to a chassis so just drop where we are...

	g_pLTServer->SetObjectPos( m_hObject, &vDropPos );

	// Go solid again if originally solid...

	g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, m_dwFlags, m_dwFlags );

	HOBJECT hFilter[] = { m_hObject, hSender, LTNULL };
	MoveObjectToFloor( m_hObject, hFilter );

	// Make sure the piece is playing it's base animation...

	HMODELANIM hAnim = 0;
	g_pModelLT->GetAnimIndex( m_hObject, "base", hAnim );
	g_pModelLT->SetCurAnim( m_hObject, MAIN_TRACKER, hAnim );
	g_pModelLT->SetLooping( m_hObject, MAIN_TRACKER, false );
	g_pModelLT->ResetAnim( m_hObject, MAIN_TRACKER );

	m_DelayTimer.Start( 0.5f );

	SetNextUpdate( UPDATE_NEXT_FRAME );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::Save
//
//	PURPOSE:	Save the object
//
// ----------------------------------------------------------------------- //

void ChassisPiece::Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags )
{
	if( !pMsg )
		return;

	m_RespawnTimer.Save( pMsg );
	SAVE_FLOAT( m_fRespawnTime );
	SAVE_CHARSTRING( m_pChassisPiece ? m_pChassisPiece->m_sName.c_str( ) : "" );
	SAVE_HOBJECT( m_hChassis );
	SAVE_VECTOR( m_vOriginalPos );
	SAVE_bool( m_bCanBeCarried );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::Load
//
//	PURPOSE:	Load the object
//
// ----------------------------------------------------------------------- //

void ChassisPiece::Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags )
{
	if( !pMsg )
		return;

	m_RespawnTimer.Load( pMsg );
	LOAD_FLOAT( m_fRespawnTime );

	char szChassisPiece[256];
	LOAD_CHARSTRING( szChassisPiece, ARRAY_LEN( szChassisPiece ));
	m_pChassisPiece = ChassisButeMgr::Instance( ).GetPiece( szChassisPiece );

	LOAD_HOBJECT( m_hChassis );
	LOAD_VECTOR( m_vOriginalPos );
	LOAD_bool( m_bCanBeCarried );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::Update
//
//	PURPOSE:	Frame Update...
//
// ----------------------------------------------------------------------- //

void ChassisPiece::Update()
{
	if( m_fRespawnTime > 0.0f )
	{
		if( m_RespawnTimer.Stopped() )
		{
			Respawn( );
			CreateSFXMessage( false, INVALID_TEAM, false );

			SetNextUpdate( UPDATE_NEVER );
		}
		else if( m_RespawnTimer.On() && m_RespawnTimer.GetCountdownTime() <= 10.0f )
		{
			// Let the clients know it will soon be back where it started...

			if( !m_bRespawnSoon )
			{
				m_bRespawnSoon = true;
				CreateSFXMessage( false, INVALID_TEAM, true );
			}

			SetNextUpdate( UPDATE_NEXT_FRAME );
		}
	}
	else
	{
		SetNextUpdate( UPDATE_NEVER );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::Respawn
//
//	PURPOSE:	Place the piece back at it's original position...
//
// ----------------------------------------------------------------------- //

bool ChassisPiece::Respawn( )
{
	LTVector vOldPos;
	g_pLTServer->GetObjectPos( m_hObject, &vOldPos );

	// Set the position to the origianl position...

	g_pLTServer->SetObjectPos( m_hObject, &m_vOriginalPos );

	m_bRespawnSoon = false;

	// Move it to the floor...

	HOBJECT hFilter[] = { m_hObject, LTNULL };
	MoveObjectToFloor( m_hObject, hFilter );

	m_RespawnTimer.Stop( );

	// Let the clients know it respawned, and where from...

	LTVector vNewPos;
	g_pLTServer->GetObjectPos( m_hObject, &vNewPos );

	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_SFX_MESSAGE );
	cMsg.Writeuint8( SFX_CHASSISPIECE_ID );
	cMsg.WriteObject( m_hObject );
	cMsg.Writebool( true );
	cMsg.WriteLTVector( vOldPos );
	cMsg.WriteLTVector( vNewPos );
	g_pLTServer->SendToClient( cMsg.Read(), LTNULL, MESSAGE_GUARANTEED );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	ChassisPiece::CreateSFXMessage
//
//	PURPOSE:	Tell the clients about the piece.
//
// ----------------------------------------------------------------------- //

void ChassisPiece::CreateSFXMessage( bool bCarried, uint8 nTeam, bool bSendToClients )
{
	bool bPlanted = !!(m_hChassis);

	CAutoMessage cMsg;
	cMsg.Writeuint8( SFX_CHASSISPIECE_ID );
	cMsg.Writeuint8( m_pChassisPiece ? m_pChassisPiece->m_nId : 0 );
	cMsg.Writebool( bCarried );
	cMsg.Writeuint8( nTeam );
	cMsg.Writebool( bPlanted );
	cMsg.Writeuint8( m_ActivateTypeHandler.GetId( ));
	cMsg.Writebool( m_bRespawnSoon );
	g_pLTServer->SetObjectSFXMessage( m_hObject, cMsg.Read( ));

	if( bSendToClients )
	{
		cMsg.Reset();
		cMsg.Writeuint8( MID_SFX_MESSAGE );
		cMsg.Writeuint8( SFX_CHASSISPIECE_ID );
		cMsg.WriteObject( m_hObject );
		cMsg.Writebool( false );
		cMsg.Writeuint8( m_pChassisPiece ? m_pChassisPiece->m_nId : 0 );
		cMsg.Writebool( bCarried );
		cMsg.Writeuint8( nTeam );
		cMsg.Writebool( bPlanted );
		cMsg.Writeuint8( m_ActivateTypeHandler.GetId( ));
		cMsg.Writebool( m_bRespawnSoon );
		g_pLTServer->SendToClient( cMsg.Read( ), LTNULL, MESSAGE_GUARANTEED );
	}
}
