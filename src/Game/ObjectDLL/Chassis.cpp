// ----------------------------------------------------------------------- //
//
// MODULE  : Chassis.cpp
//
// PURPOSE : A prop that chassis pieces are carried to and put on.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from Object.lto
// (0x100abf80..0x100ade30), starting from NOLF2's DoomsDayDevice.
//
// ----------------------------------------------------------------------- //

//
// Includes...
//

#include "stdafx.h"
#include "ParsedMsg.h"
#include "Chassis.h"
#include "ChassisPiece.h"
#include "ChassisButeMgr.h"
#include "PlayerObj.h"
#include "Spawner.h"

LINKFROM_MODULE( Chassis );

BEGIN_CLASS( Chassis )
	ADD_DESTRUCTIBLE_MODEL_AGGREGATE(PF_GROUP(1), PF_HIDDEN)
	ADD_VECTORPROP_VAL_FLAG(Scale, 1.0f, 1.0f, 1.0f, PF_HIDDEN)
	ADD_BOOLPROP_FLAG(CanTransition, LTFALSE, PF_HIDDEN)
	ADD_REALPROP_FLAG( DropZoneRadius, 64.0f, PF_RADIUS )
	ADD_STRINGPROP_FLAG( AllowedPieces, "", PF_NOTIFYCHANGE )
	ADD_STRINGPROP_FLAG( Team, "NoTeam", PF_STATICLIST )
	ADD_STRINGPROP_FLAG( OnComplete, "", PF_NOTIFYCHANGE )
END_CLASS_DEFAULT_FLAGS_PLUGIN( Chassis, PropType, NULL, NULL, 0, CChassisPlugin )


// Register with the CommandMgr...

CMDMGR_BEGIN_REGISTER_CLASS( Chassis )
	CMDMGR_ADD_MSG( TEAM, 2, NULL, "TEAM <0, 1, -1>" )
CMDMGR_END_REGISTER_CLASS( Chassis, PropType )

Chassis::ChassisList Chassis::m_lstChassis;

LTRESULT CChassisPlugin::PreHook_EditStringList(
	const char *szRezPath,
	const char *szPropName,
	char **aszStrings,
	uint32 *pcStrings,
	const uint32 cMaxStrings,
	const uint32 cMaxStringLen )
{
	// Handle team...

	if( _stricmp( "Team", szPropName ) == 0 )
	{
		char szTeam[32] = {0};

		_ASSERT(cMaxStrings > (*pcStrings) + 1);
		strcpy( aszStrings[(*pcStrings)++], "NoTeam" );

		for( int i = 0; i < MAX_TEAMS; ++i )
		{
			_ASSERT(cMaxStrings > (*pcStrings) + 1);

			sprintf( szTeam, "Team%i", i );
			strcpy( aszStrings[(*pcStrings)++], szTeam );
		}

		return LT_OK;
	}

	if( LT_OK == CPropTypePlugin::PreHook_EditStringList( szRezPath, szPropName, aszStrings, pcStrings, cMaxStrings, cMaxStringLen ))
	{
		return LT_OK;
	}

	return LT_UNSUPPORTED;
}

LTRESULT CChassisPlugin::PreHook_PropChanged(
	const char *szObjName,
	const char *szPropName,
	const int nPropType,
	const GenericProp &gpPropValue,
	ILTPreInterface *pInterface,
	const char *szModifiers )
{
	// Every piece allowed must be a PropType...

	if( !_stricmp( szPropName, "AllowedPieces" ) && gpPropValue.m_String[0] )
	{
		ConParse cpPieces;
		cpPieces.Init( gpPropValue.m_String );

		while( pInterface->Parse( &cpPieces ) == LT_OK )
		{
			for( int i = 0; i < cpPieces.m_nArgs; ++i )
			{
				if( !g_pPropTypeMgr->GetPropType( cpPieces.m_Args[i] ))
					return LT_UNSUPPORTED;
			}
		}

		return LT_OK;
	}

	if( !_stricmp( szPropName, "OnComplete" ) && gpPropValue.m_String[0] )
	{
		if( m_CommandMgrPlugin.PreHook_PropChanged( szObjName, szPropName, nPropType, gpPropValue, pInterface, szModifiers ) == LT_OK )
			return LT_OK;
	}

	return CPropPlugin::PreHook_PropChanged( szObjName, szPropName, nPropType, gpPropValue, pInterface, szModifiers );
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	Chassis::Chassis
//
//  PURPOSE:	Constructor...
//
// ----------------------------------------------------------------------- //

Chassis::Chassis( )
:	PropType			( ),
	m_nOwningTeamID		( INVALID_TEAM ),
	m_fDropZoneRadius	( 64.0f )
{
	m_lstChassis.push_back( this );
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	Chassis::~Chassis
//
//  PURPOSE:	Destructor...
//
// ----------------------------------------------------------------------- //

Chassis::~Chassis( )
{
	ClearSlots( );

	for( ChassisList::iterator iter = m_lstChassis.begin( ); iter != m_lstChassis.end( ); ++iter )
	{
		if( *iter == this )
		{
			m_lstChassis.erase( iter );
			break;
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::EngineMessageFn
//
//	PURPOSE:	Handle engine messages
//
// ----------------------------------------------------------------------- //

uint32 Chassis::EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData )
{
	switch( messageID )
	{
		case MID_PRECREATE:
		{
			uint32 dwRet = PropType::EngineMessageFn( messageID, pData, fData );

			if( fData == PRECREATE_WORLDFILE || fData == PRECREATE_STRINGPROP )
			{
				if( !ReadProp(( ObjectCreateStruct* )pData ))
					return 0;
			}

			return dwRet;
		}
		break;

		case MID_INITIALUPDATE:
		{
			uint32 dwRet = PropType::EngineMessageFn( messageID, pData, fData );

			if( fData != INITIALUPDATE_SAVEGAME )
			{
				CreateSlotModels( );
			}

			CreateSFXMessage( );

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
//	ROUTINE:	Chassis::OnTrigger
//
//	PURPOSE:	Handle trigger message.
//
// ----------------------------------------------------------------------- //

bool Chassis::OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg )
{
	static CParsedMsg::CToken	s_cTok_Team( "TEAM" );

	if( cMsg.GetArg( 0 ) == s_cTok_Team && cMsg.GetArgCount( ) > 1 )
	{
		uint32 nTeamId = atoi( cMsg.GetArg( 1 ));
		if( nTeamId < MAX_TEAMS )
		{
			m_nOwningTeamID = nTeamId;
		}
		else
		{
			m_nOwningTeamID = INVALID_TEAM;
		}

		return true;
	}

	return PropType::OnTrigger( hSender, cMsg );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::ReadProp
//
//	PURPOSE:	Reads properties.
//
// ----------------------------------------------------------------------- //

bool Chassis::ReadProp( ObjectCreateStruct* pStruct )
{
	GenericProp genProp;

	if( IsTeamGameType() )
	{
		if( g_pLTServer->GetPropGeneric( "Team", &genProp ) == LT_OK )
		{
			if( genProp.m_String[0] )
			{
				// The team string should be TeamN, when N is the team id.
				char const szTeam[] = "Team";
				int nLen = strlen( szTeam );
				if( !_strnicmp( genProp.m_String, szTeam, nLen ))
				{
					uint32 nTeamId = atoi( &genProp.m_String[ nLen ] );
					if( nTeamId < MAX_TEAMS )
					{
						m_nOwningTeamID = nTeamId;
					}
				}
			}
		}
	}

	// Get the diminsions considered to be the drop zone...

	if( g_pLTServer->GetPropGeneric( "DropZoneRadius", &genProp ) == LT_OK )
	{
		m_fDropZoneRadius = genProp.m_Float;
	}

	// A slot for each piece the chassis takes...

	if( g_pLTServer->GetPropGeneric( "AllowedPieces", &genProp ) == LT_OK && genProp.m_String[0] )
	{
		ConParse cpPieces;
		cpPieces.Init( genProp.m_String );

		while( g_pCommonLT->Parse( &cpPieces ) == LT_OK )
		{
			for( int i = 0; i < cpPieces.m_nArgs; ++i )
			{
				CHASSISPIECE const* pType = ChassisButeMgr::Instance( ).GetPiece( cpPieces.m_Args[i] );
				if( pType )
				{
					Slot slot;
					slot.m_pType = pType;
					m_Slots.push_back( slot );
				}
			}
		}
	}

	if( g_pLTServer->GetPropGeneric( "OnComplete", &genProp ) == LT_OK && genProp.m_String[0] )
	{
		m_sOnComplete = genProp.m_String;
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::CreateSlotModels
//
//	PURPOSE:	Create the see-through model of each piece, showing where
//				it goes.
//
// ----------------------------------------------------------------------- //

void Chassis::CreateSlotModels( )
{
	char szSpawn[256];

	LTVector vPos;
	g_pLTServer->GetObjectPos( m_hObject, &vPos );

	LTRotation rRot;
	g_pLTServer->GetObjectRotation( m_hObject, &rRot );

	for( SlotList::iterator iter = m_Slots.begin( ); iter != m_Slots.end( ); ++iter )
	{
		CHASSISPIECE const* pType = iter->m_pType;
		if( pType->m_sPropTypeTarget.empty( ))
			continue;

		sprintf( szSpawn, "PropType Type %s", pType->m_sPropTypeTarget.c_str( ));

		BaseClass* pObj = SpawnObject( szSpawn, vPos, rRot );
		if( !pObj || !pObj->m_hObject )
			continue;

		iter->m_hModel = pObj->m_hObject;

		g_pCommonLT->SetObjectFlags( iter->m_hModel, OFT_Flags2, FLAG2_FORCETRANSLUCENT, FLAG2_FORCETRANSLUCENT );

		// Attach the model to the chassis...

		HATTACHMENT hAttachment;
		g_pLTServer->CreateAttachment( m_hObject, iter->m_hModel, iter->m_pType->m_sChassisSocket.c_str( ), &LTVector(0,0,0), &LTRotation(), &hAttachment );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::IsComplete
//
//	PURPOSE:	Are all the slots filled?
//
// ----------------------------------------------------------------------- //

bool Chassis::IsComplete( ) const
{
	for( SlotList::const_iterator iter = m_Slots.begin( ); iter != m_Slots.end( ); ++iter )
	{
		if( !iter->m_hPiece )
			return false;
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::FindSlot
//
//	PURPOSE:	The slot a piece type goes in.
//
// ----------------------------------------------------------------------- //

Chassis::Slot* Chassis::FindSlot( CHASSISPIECE const* pType )
{
	for( SlotList::iterator iter = m_Slots.begin( ); iter != m_Slots.end( ); ++iter )
	{
		if( !iter->m_pType )
			break;

		if( iter->m_pType == pType )
			return &(*iter);
	}

	return LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::AddPiece
//
//	PURPOSE:	Put a piece in its slot...
//
// ----------------------------------------------------------------------- //

bool Chassis::AddPiece( ChassisPiece *pPiece, CPlayerObj *pPlayer )
{
	if( IsComplete( ))
		return false;

	if( !pPiece->GetChassisPiece( ))
		return false;

	Slot* pSlot = FindSlot( pPiece->GetChassisPiece( ));
	if( !pSlot )
		return false;

	if( pSlot->m_hPiece )
		return false;

	// Reset the filenames to the 'replaced' PropType...

	if( !pSlot->m_pType->m_sPropTypeReplaced.empty( ))
	{
		PROPTYPE *pPropType = g_pPropTypeMgr->GetPropType(( char* )pSlot->m_pType->m_sPropTypeReplaced.c_str( ));
		if( !pPropType )
			return false;

		ObjectCreateStruct ocs;

		LTStrCpy( ocs.m_Filename, pPropType->sFilename.c_str(), ARRAY_LEN( ocs.m_Filename ));

		pPropType->blrPropRenderStyleReader.CopyList( 0, ocs.m_RenderStyleNames[0], MAX_CS_FILENAME_LEN + 1 );
		pPropType->blrPropSkinReader.CopyList( 0, ocs.m_SkinNames[0], MAX_CS_FILENAME_LEN + 1 );

		if( g_pCommonLT->SetObjectFilenames( pPiece->m_hObject, &ocs ) != LT_OK )
			return false;

		// Attach the piece to the chassis...

		LTVector vPos;
		g_pLTServer->GetObjectPos( m_hObject, &vPos );
		g_pLTServer->SetObjectPos( pPiece->m_hObject, &vPos );

		LTRotation rRot;
		g_pLTServer->GetObjectRotation( m_hObject, &rRot );
		g_pLTServer->SetObjectRotation( pPiece->m_hObject, &rRot );

		HATTACHMENT hAttachment;
		g_pLTServer->CreateAttachment( m_hObject, pPiece->m_hObject, pSlot->m_pType->m_sChassisSocket.c_str( ), &LTVector(0,0,0), &LTRotation(), &hAttachment );
	}

	// Hide the model showing where it goes...

	if( pSlot->m_hModel )
	{
		g_pCommonLT->SetObjectFlags( pSlot->m_hModel, OFT_Flags, 0, FLAG_VISIBLE );
	}

	pSlot->m_hPiece = pPiece->m_hObject;

	if( IsComplete( ))
	{
		const char *pCmd = m_sOnComplete.c_str( );
		if( g_pCmdMgr->IsValidCmd( pCmd ))
		{
			g_pCmdMgr->Process( pCmd, m_hObject, LTNULL );
		}
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::RemovePiece
//
//	PURPOSE:	Take a piece out of its slot...
//
// ----------------------------------------------------------------------- //

bool Chassis::RemovePiece( ChassisPiece *pPiece, CPlayerObj *pPlayer )
{
	CHASSISPIECE const* pType = pPiece->GetChassisPiece( );
	if( !pType )
		return false;

	Slot* pSlot = FindSlot( pType );
	if( !pSlot || !pSlot->m_hPiece || pSlot->m_hPiece != pPiece->m_hObject )
		return false;

	PROPTYPE *pPropType = g_pPropTypeMgr->GetPropType(( char* )pSlot->m_pType->m_sPropType.c_str( ));
	if( !pPropType )
		return false;

	pSlot->m_hPiece = LTNULL;

	// Detach the piece from the chassis...

	HATTACHMENT hAttachment;
	if( g_pLTServer->FindAttachment( m_hObject, pPiece->m_hObject, &hAttachment ) == LT_OK )
	{
		g_pLTServer->RemoveAttachment( hAttachment );
	}

	// Reset the filenames to the normal PropType...

	ObjectCreateStruct ocs;

	LTStrCpy( ocs.m_Filename, pPropType->sFilename.c_str(), ARRAY_LEN( ocs.m_Filename ));

	pPropType->blrPropRenderStyleReader.CopyList( 0, ocs.m_RenderStyleNames[0], MAX_CS_FILENAME_LEN + 1 );
	pPropType->blrPropSkinReader.CopyList( 0, ocs.m_SkinNames[0], MAX_CS_FILENAME_LEN + 1 );

	g_pCommonLT->SetObjectFilenames( pPiece->m_hObject, &ocs );

	// Show the model of where it goes again...

	if( pSlot->m_hModel )
	{
		g_pCommonLT->SetObjectFlags( pSlot->m_hModel, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE );
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::ClearSlots
//
//	PURPOSE:	Remove the slots, and the models and pieces in them.
//
// ----------------------------------------------------------------------- //

void Chassis::ClearSlots( )
{
	for( SlotList::iterator iter = m_Slots.begin( ); iter != m_Slots.end( ); ++iter )
	{
		HATTACHMENT hAttachment = LTNULL;
		if( g_pLTServer->FindAttachment( m_hObject, iter->m_hModel, &hAttachment ) == LT_OK )
		{
			g_pLTServer->RemoveAttachment( hAttachment );
		}

		if( iter->m_hModel )
		{
			g_pLTServer->RemoveObject( iter->m_hModel );
		}

		if( iter->m_hPiece )
		{
			hAttachment = LTNULL;
			if( g_pLTServer->FindAttachment( m_hObject, iter->m_hPiece, &hAttachment ) == LT_OK )
			{
				g_pLTServer->RemoveAttachment( hAttachment );
			}

			g_pLTServer->RemoveObject( iter->m_hPiece );
		}
	}

	m_Slots.clear( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::CreateSFXMessage
//
//	PURPOSE:	Tell the clients about the chassis.
//
// ----------------------------------------------------------------------- //

void Chassis::CreateSFXMessage( )
{
	CAutoMessage cMsg;
	cMsg.Writeuint8( SFX_CHASSIS_ID );
	cMsg.Writeuint8( m_nOwningTeamID );
	cMsg.Writefloat( m_fDropZoneRadius );
	cMsg.Writeuint8( m_ActivateTypeHandler.GetId( ));
	g_pLTServer->SetObjectSFXMessage( m_hObject, cMsg.Read( ));

	cMsg.Reset( );
	cMsg.Writeuint8( MID_SFX_MESSAGE );
#ifdef CJACK_MODERN
	// MODERN.md GAME-2: say which special fx the update is for.
	cMsg.Writeuint8( SFX_CHASSIS_ID );
	cMsg.WriteObject( m_hObject );
#else
	// RETAIL: 0x100aca0f the update sent to the clients has no SFX id or
	// object in it, so a client reads the team as the SFX id (RETAIL_BUGS.md).
#endif
	cMsg.Writeuint8( m_nOwningTeamID );
	cMsg.Writefloat( m_fDropZoneRadius );
	cMsg.Writeuint8( m_ActivateTypeHandler.GetId( ));
	g_pLTServer->SendToClient( cMsg.Read( ), LTNULL, MESSAGE_GUARANTEED );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::Save
//
//	PURPOSE:	Save the object
//
// ----------------------------------------------------------------------- //

void Chassis::Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags )
{
	if( !pMsg )
		return;

	SAVE_BYTE( m_nOwningTeamID );
	SAVE_FLOAT( m_fDropZoneRadius );
	SAVE_CHARSTRING( m_sOnComplete.c_str( ));

	SAVE_DWORD( m_Slots.size( ));
	for( SlotList::iterator iter = m_Slots.begin( ); iter != m_Slots.end( ); ++iter )
	{
		SAVE_CHARSTRING( iter->m_pType->m_sName.c_str( ));
		SAVE_HOBJECT( iter->m_hModel );
		SAVE_HOBJECT( iter->m_hPiece );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	Chassis::Load
//
//	PURPOSE:	Load the object
//
// ----------------------------------------------------------------------- //

void Chassis::Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags )
{
	if( !pMsg )
		return;

	char szString[256];

	LOAD_BYTE( m_nOwningTeamID );
	LOAD_FLOAT( m_fDropZoneRadius );
	LOAD_CHARSTRING( szString, ARRAY_LEN( szString ));
	m_sOnComplete = szString;

	uint32 nSlots;
	LOAD_DWORD( nSlots );

	ClearSlots( );

	for( uint32 i = 0; i < nSlots; ++i )
	{
		Slot slot;

		LOAD_CHARSTRING( szString, ARRAY_LEN( szString ));
		slot.m_pType = ChassisButeMgr::Instance( ).GetPiece( szString );
		if( slot.m_pType )
		{
			LOAD_HOBJECT( slot.m_hModel );
			LOAD_HOBJECT( slot.m_hPiece );
			m_Slots.push_back( slot );
		}
	}
}
