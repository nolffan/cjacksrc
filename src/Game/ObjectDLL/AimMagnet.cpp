// ----------------------------------------------------------------------- //
//
// MODULE  : AimMagnet.cpp
//
// PURPOSE : AimMagnet - Implementation
//
// CREATED : RETAIL: Contract J.A.C.K.'s (Object 0x10059520..0x1005a240)
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "AimMagnet.h"
#include "ServerUtilities.h"
#include "ParsedMsg.h"
#include "CommandMgr.h"
#include "MsgIDs.h"
#include "SFXMsgIds.h"
#include "TeamMgr.h"

LINKFROM_MODULE( AimMagnet );

AimMagnet::AimMagnetList AimMagnet::m_lstAimMagnets;

#pragma force_active on
BEGIN_CLASS(AimMagnet)
	ADD_BOOLPROP_FLAG(Solid, LTFALSE, PF_HIDDEN)
	ADD_BOOLPROP_FLAG(Gravity, LTFALSE, PF_HIDDEN)
	ADD_STRINGPROP_FLAG(Target, "", PF_OBJECTLINK)
	ADD_STRINGPROP_FLAG(Team, "NoTeam", PF_STATICLIST | PF_HIDDEN)
END_CLASS_DEFAULT_FLAGS_PLUGIN(AimMagnet, GameBase, NULL, NULL, 0, CAimMagnetPlugin)
#pragma force_active off

CMDMGR_BEGIN_REGISTER_CLASS( AimMagnet )

	CMDMGR_ADD_MSG( TEAM,	2,	NULL,	"TEAM <0, 1, -1>" )
	CMDMGR_ADD_MSG( ON,		1,	NULL,	"ON" )
	CMDMGR_ADD_MSG( OFF,	1,	NULL,	"OFF" )
	CMDMGR_ADD_MSG( TARGET,	2,	NULL,	"TARGET <objectname>" )

CMDMGR_END_REGISTER_CLASS( AimMagnet, GameBase )

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CAimMagnetPlugin::PreHook_EditStringList
//
//	PURPOSE:	RETAIL: Object 0x10059530: the team list
//
// ----------------------------------------------------------------------- //

LTRESULT CAimMagnetPlugin::PreHook_EditStringList(
	const char* szRezPath,
	const char* szPropName,
	char** aszStrings,
    uint32* pcStrings,
    const uint32 cMaxStrings,
    const uint32 cMaxStringLength)
{
	if( _stricmp( "Team", szPropName ) == 0 )
	{
		char szTeam[32] = {0};

		strcpy( aszStrings[(*pcStrings)++], "NoTeam" );

		for( int i = 0; i < MAX_TEAMS; ++i )
		{
			sprintf( szTeam, "Team%i", i );
			strcpy( aszStrings[(*pcStrings)++], szTeam );
		}

		return LT_OK;
	}

	return LT_UNSUPPORTED;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::InitialUpdate
//
//	PURPOSE:	RETAIL: Object 0x10059600
//
// ----------------------------------------------------------------------- //

LTBOOL AimMagnet::InitialUpdate( )
{
	SetNextUpdate( UPDATE_NEVER );

	g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE );

	CreateSpecialFX( false );

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::Update
//
//	PURPOSE:	RETAIL: Object 0x10059640: tell the clients once the
//				target is known
//
// ----------------------------------------------------------------------- //

LTBOOL AimMagnet::Update( )
{
	SetNextUpdate( UPDATE_NEVER );

	CreateSpecialFX( true );

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::SetTeam
//
//	PURPOSE:	RETAIL: Object 0x10059660
//
// ----------------------------------------------------------------------- //

void AimMagnet::SetTeam( uint8 nTeamId )
{
	m_nTeamId = nTeamId;
	CreateSpecialFX( true );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::OnLinkBroken
//
//	PURPOSE:	RETAIL: Object 0x10059680
//
// ----------------------------------------------------------------------- //

void AimMagnet::OnLinkBroken( LTObjRefNotifier *pRef, HOBJECT hObj )
{
	if( pRef == &m_hTarget )
	{
		CreateSpecialFX( true );
	}

	g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_VISIBLE );

	GameBase::OnLinkBroken( pRef, hObj );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::Save
//
//	PURPOSE:	RETAIL: Object 0x100596d0
//
// ----------------------------------------------------------------------- //

void AimMagnet::Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags )
{
	if( !pMsg ) return;

	SAVE_BYTE( m_nTeamId );
	pMsg->WriteString( m_sTargetName.c_str( ));
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::ReadProp
//
//	PURPOSE:	RETAIL: Object 0x10059710
//
// ----------------------------------------------------------------------- //

LTBOOL AimMagnet::ReadProp( ObjectCreateStruct *pStruct )
{
	GenericProp genProp;

	if( !pStruct ) return LTFALSE;

	if( g_pLTServer->GetPropGeneric( "Target", &genProp ) == LT_OK )
	{
		if( genProp.m_String[0] )
		{
			m_sTargetName = genProp.m_String;
		}
	}

	if( IsTeamGameType( ))
	{
		if( g_pLTServer->GetPropGeneric( "Team", &genProp ) == LT_OK )
		{
			m_nTeamId = TeamStringToTeamId( genProp.m_String );
		}
	}
	else
	{
		m_nTeamId = INVALID_TEAM;
	}

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::CreateSpecialFX
//
//	PURPOSE:	RETAIL: Object 0x10059810
//
// ----------------------------------------------------------------------- //

void AimMagnet::CreateSpecialFX( bool bUpdateClients )
{
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8( SFX_AIMMAGNET_ID );
		cMsg.Writeuint8( m_nTeamId );
		cMsg.WriteObject( m_hTarget );
		g_pLTServer->SetObjectSFXMessage( m_hObject, cMsg.Read( ));
	}

	if( bUpdateClients )
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8( MID_SFX_MESSAGE );
		cMsg.Writeuint8( SFX_AIMMAGNET_ID );
		cMsg.WriteObject( m_hObject );
		cMsg.Writeuint8( m_nTeamId );
		cMsg.WriteObject( m_hTarget );
		g_pLTServer->SendToClient( cMsg.Read( ), LTNULL, MESSAGE_GUARANTEED );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::ResolveTarget
//
//	PURPOSE:	RETAIL: Object 0x10059980: find the object to follow
//
// ----------------------------------------------------------------------- //

void AimMagnet::ResolveTarget( )
{
	if( m_hTarget || m_sTargetName.empty( )) return;

	HOBJECT hTarget = LTNULL;
	if( FindNamedObject( m_sTargetName.c_str( ), hTarget ) != LT_OK )
	{
		// Nothing to follow, so nothing to pull to.
		m_hTarget = LTNULL;

		g_pLTServer->RemoveObject( m_hObject );
		return;
	}

	m_hTarget = hTarget;

	GameBase* pTarget = dynamic_cast< GameBase* >( g_pLTServer->HandleToObject( m_hTarget ));
	if( pTarget && pTarget->CanTransition( ))
	{
		g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, FLAG_TOUCH_NOTIFY, FLAG_TOUCH_NOTIFY );
	}

	SetNextUpdate( UPDATE_NEXT_FRAME );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::~AimMagnet
//
//	PURPOSE:	RETAIL: Object 0x10059a70
//
// ----------------------------------------------------------------------- //

AimMagnet::~AimMagnet()
{
	// RETAIL: Object 0x10059a80: a search loop that erases what it finds.
	AimMagnetList::iterator iter = m_lstAimMagnets.begin( );
	while( iter != m_lstAimMagnets.end( ))
	{
		if( *iter == this )
		{
			m_lstAimMagnets.erase( iter );
			break;
		}

		iter++;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::OnTrigger
//
//	PURPOSE:	RETAIL: Object 0x10059ae0
//
// ----------------------------------------------------------------------- //

bool AimMagnet::OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg )
{
	static CParsedMsg::CToken s_cTok_Team( "TEAM" );
	static CParsedMsg::CToken s_cTok_Target( "TARGET" );
	static CParsedMsg::CToken s_cTok_On( "ON" );
	static CParsedMsg::CToken s_cTok_Off( "OFF" );

	if( cMsg.GetArg(0) == s_cTok_Team )
	{
		if( cMsg.GetArgCount( ) > 1 )
		{
			uint32 nTeamId = atoi( cMsg.GetArg( 1 ));
			// RETAIL: Object 0x10059bdb: written as if/else.
			if( nTeamId < MAX_TEAMS )
				SetTeam( nTeamId );
			else
				SetTeam( INVALID_TEAM );
		}
	}
	else if( cMsg.GetArg(0) == s_cTok_Target )
	{
		if( cMsg.GetArgCount( ) > 1 )
		{
			m_sTargetName = cMsg.GetArg( 1 ).c_str( );
			m_hTarget = LTNULL;

			ResolveTarget( );
		}
	}
	else if( cMsg.GetArg(0) == s_cTok_On )
	{
		if( m_hObject )
		{
			g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE );
		}
	}
	else if( cMsg.GetArg(0) == s_cTok_Off )
	{
		if( m_hObject )
		{
			g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_VISIBLE );
		}
	}
	else
	{
		return GameBase::OnTrigger( hSender, cMsg );
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::Load
//
//	PURPOSE:	RETAIL: Object 0x10059d10
//
// ----------------------------------------------------------------------- //

void AimMagnet::Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags )
{
	if( !pMsg ) return;

	LOAD_BYTE( m_nTeamId );

	char szTarget[128];
	pMsg->ReadString( szTarget, ARRAY_LEN( szTarget ));
	m_sTargetName = szTarget;

	ResolveTarget( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::EngineMessageFn
//
//	PURPOSE:	RETAIL: Object 0x10059d70
//
// ----------------------------------------------------------------------- //

uint32 AimMagnet::EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData )
{
	switch( messageID )
	{
		case MID_UPDATE:
		{
			Update( );
		}
		break;

		case MID_PRECREATE:
		{
			if( fData == PRECREATE_WORLDFILE )
			{
				ReadProp( (ObjectCreateStruct*)pData );
			}
			else if( fData == PRECREATE_STRINGPROP )
			{
				ReadProp( (ObjectCreateStruct*)pData );
				((ObjectCreateStruct*)pData)->m_Flags |= FLAG_VISIBLE;
			}
		}
		break;

		case MID_INITIALUPDATE:
		{
			if( fData != INITIALUPDATE_SAVEGAME )
			{
				InitialUpdate( );
			}
		}
		break;

		case MID_ALLOBJECTSCREATED:
		{
			ResolveTarget( );
		}
		break;

		case MID_SAVEOBJECT:
		{
			Save( (ILTMessage_Write*)pData, (uint32)fData );
		}
		break;

		case MID_LOADOBJECT:
		{
			Load( (ILTMessage_Read*)pData, (uint32)fData );
		}
		break;

		default : break;
	}

	return GameBase::EngineMessageFn( messageID, pData, fData );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	AimMagnet::AimMagnet
//
//	PURPOSE:	RETAIL: Object 0x10059ef0
//
// ----------------------------------------------------------------------- //

AimMagnet::AimMagnet() : GameBase( OT_NORMAL ),
	m_hTarget( ( HOBJECT )LTNULL )	// RETAIL: Object 0x10059f2a links it to nothing
{
	MakeTransitionable();

	m_hTarget.SetReceiver( *this );
	m_nTeamId = INVALID_TEAM;

	m_lstAimMagnets.push_back( this );
}
