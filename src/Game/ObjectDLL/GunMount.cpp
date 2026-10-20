// ----------------------------------------------------------------------- //
//
// MODULE  : GunMount.cpp
//
// PURPOSE : GunMount - Implementation
//
// CREATED : RETAIL: Contract J.A.C.K.'s (Object 0x100e4b70..0x100e5d90)
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "GunMount.h"
#include "ServerUtilities.h"
#include "ParsedMsg.h"
#include "CommandMgr.h"
#include "Weapon.h"
#include "WeaponFireInfo.h"
#include "WeaponMgr.h"

LINKFROM_MODULE( GunMount );

#pragma force_active on
BEGIN_CLASS(GunMount)
	ADD_STRINGPROP_FLAG(Filename, "", PF_HIDDEN | PF_MODEL)
	ADD_STRINGPROP_FLAG(Weapon, "<none>", PF_STATICLIST | PF_DIMS | PF_LOCALDIMS)
	ADD_BOOLPROP(Hidden, LTFALSE)
END_CLASS_DEFAULT_FLAGS_PLUGIN(GunMount, GameBase, NULL, NULL, 0, GunMountPlugin)
#pragma force_active off

// RETAIL: Object 0x100e4b70: FIRE takes an optional round count.
static LTBOOL ValidateFireMsg( ILTPreInterface *pInterface, ConParse &cpMsgParams )
{
	return !!pInterface;
}

CMDMGR_BEGIN_REGISTER_CLASS( GunMount )

	CMDMGR_ADD_MSG( FIRE,		-1,	ValidateFireMsg,	"FIRE [rounds_to_fire]" )
	CMDMGR_ADD_MSG( BEGINFIRE,	1,	NULL,				"BEGINFIRE" )
	CMDMGR_ADD_MSG( ENDFIRE,	1,	NULL,				"ENDFIRE" )

CMDMGR_END_REGISTER_CLASS( GunMount, GameBase )

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMountPlugin::PreHook_EditStringList
//
//	PURPOSE:	RETAIL: Object 0x100e4e40: the weapon list
//
// ----------------------------------------------------------------------- //

LTRESULT GunMountPlugin::PreHook_EditStringList(
	const char* szRezPath,
	const char* szPropName,
	char** aszStrings,
    uint32* pcStrings,
    const uint32 cMaxStrings,
    const uint32 cMaxStringLength)
{
	static CWeaponMgrPlugin s_WeaponMgrPlugin;
	static bool s_bInitted = false;

	if( !s_bInitted )
	{
		s_bInitted = true;
		s_WeaponMgrPlugin.PreHook_EditStringList( szRezPath, szPropName, aszStrings, pcStrings, cMaxStrings, cMaxStringLength );
	}

	if( _stricmp( "Weapon", szPropName ) == 0 )
	{
		strcpy( aszStrings[(*pcStrings)++], "<none>" );
		s_WeaponMgrPlugin.PopulateStringList( aszStrings, pcStrings, cMaxStrings, cMaxStringLength );
		return LT_OK;
	}

	return LT_UNSUPPORTED;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMountPlugin::PreHook_Dims
//
//	PURPOSE:	RETAIL: Object 0x100e4f10: show the weapon's model
//
// ----------------------------------------------------------------------- //

LTRESULT GunMountPlugin::PreHook_Dims(
	const char* szRezPath,
	const char* szPropValue,
	char* szModelFilenameBuf,
	int	  nModelFilenameBufLen,
	LTVector & vDims)
{
	if( !szModelFilenameBuf || nModelFilenameBufLen < 1 || !g_pWeaponMgr )
		return LT_UNSUPPORTED;

	szModelFilenameBuf[0] = '\0';

	char szWeapon[256];
	strncpy( szWeapon, szPropValue, ARRAY_LEN( szWeapon ) - 1 );
	szWeapon[ARRAY_LEN( szWeapon ) - 1] = '\0';
	strtok( szWeapon, "," );

	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon( szWeapon );
	if( !pWeapon || !pWeapon->szHHModel[0] )
		return LT_UNSUPPORTED;

	strcpy( szModelFilenameBuf, pWeapon->szHHModel );
	ConvertLTBFilename( szModelFilenameBuf );

	return LT_OK;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::GunMount
//
//	PURPOSE:	RETAIL: Object 0x100e4b80
//
// ----------------------------------------------------------------------- //

GunMount::GunMount() : GameBase( OT_NORMAL )
{
	m_eFiringState = eNotFiring;
	m_nRoundsToFire = 0;
	m_bVisible = true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::~GunMount
//
//	PURPOSE:	RETAIL: Object 0x100e4d90
//
// ----------------------------------------------------------------------- //

GunMount::~GunMount()
{
	// The weapon's model goes with us.
	g_pLTServer->RemoveObject( m_Weapons.GetWeaponModel( ));
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::EngineMessageFn
//
//	PURPOSE:	RETAIL: Object 0x100e5c00
//
// ----------------------------------------------------------------------- //

uint32 GunMount::EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData )
{
	switch( messageID )
	{
		case MID_PRECREATE:
		{
			uint32 dwRet = GameBase::EngineMessageFn( messageID, pData, fData );

			if( fData == PRECREATE_WORLDFILE || fData == PRECREATE_STRINGPROP )
			{
				if( !ReadProp( (ObjectCreateStruct*)pData ))
					return 0;
			}

			return dwRet;
		}
		break;

		case MID_INITIALUPDATE:
		{
			uint32 dwRet = GameBase::EngineMessageFn( messageID, pData, fData );

			if( fData != INITIALUPDATE_SAVEGAME )
			{
				InitialUpdate( );
			}

			return dwRet;
		}
		break;

		case MID_UPDATE:
		{
			Update( );
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
//	ROUTINE:	GunMount::ObjectMessageFn
//
//	PURPOSE:	RETAIL: Object 0x100e4bd0
//
// ----------------------------------------------------------------------- //

uint32 GunMount::ObjectMessageFn( HOBJECT hSender, ILTMessage_Read *pMsg )
{
	GameBase::ObjectMessageFn( hSender, pMsg );
	m_Weapons.ObjectMessageFn( this, hSender, pMsg );
	return 0;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::ReadProp
//
//	PURPOSE:	RETAIL: Object 0x100e4fe0
//
// ----------------------------------------------------------------------- //

LTBOOL GunMount::ReadProp( ObjectCreateStruct *pStruct )
{
	GenericProp genProp;

	if( g_pLTServer->GetPropGeneric( "Weapon", &genProp ) == LT_OK )
	{
		if( genProp.m_String[0] )
		{
			m_sWeapon = genProp.m_String;
		}
	}

	if( g_pLTServer->GetPropGeneric( "Hidden", &genProp ) == LT_OK )
	{
		m_bVisible = !genProp.m_Bool;
	}

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::InitialUpdate
//
//	PURPOSE:	RETAIL: Object 0x100e5520
//
// ----------------------------------------------------------------------- //

void GunMount::InitialUpdate( )
{
	g_pCommonLT->SetObjectFlags( m_hObject, OFT_Flags, FLAG_FULLPOSITIONRES | FLAG_FORCECLIENTUPDATE, FLAG_FULLPOSITIONRES | FLAG_FORCECLIENTUPDATE );

	// The weapon models point the other way.

	LTRotation rRot;
	g_pLTServer->GetObjectRotation( m_hObject, &rRot );
	rRot.Rotate( rRot.Up( ), MATH_PI );
	g_pLTServer->SetObjectRotation( m_hObject, &rRot );

	CreateWeapon( );
	SetVisible( m_bVisible );

	SetNextUpdate( UPDATE_NEXT_FRAME );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::CreateWeapon
//
//	PURPOSE:	RETAIL: Object 0x100e5170: the weapon and its model
//
// ----------------------------------------------------------------------- //

bool GunMount::CreateWeapon( )
{
	uint8 nWeaponId = WMGR_INVALID_ID;
	uint8 nAmmoId = WMGR_INVALID_ID;
	g_pWeaponMgr->ReadWeapon( (char*)m_sWeapon.c_str( ), nWeaponId, nAmmoId );

	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon( nWeaponId );
	if( !pWeapon ) return false;

	ObjectCreateStruct theStruct;
	INIT_OBJECTCREATESTRUCT( theStruct );

	theStruct.m_Flags = FLAG_VISIBLE | FLAG_GOTHRUWORLD;
	theStruct.m_Flags2 |= FLAG2_DYNAMICDIRLIGHT;
	theStruct.m_NextUpdate = 1.0f;

	pWeapon->blrHHSkins.CopyList( 0, theStruct.m_SkinNames[0], MAX_CS_FILENAME_LEN+1 );
	pWeapon->blrHHRenderStyles.CopyList( 0, theStruct.m_RenderStyleNames[0], MAX_CS_FILENAME_LEN+1 );
	SAFE_STRCPY( theStruct.m_Filename, pWeapon->szHHModel );

	HCLASS hClass = g_pLTServer->GetClass( "CHHWeaponModel" );
	LPBASECLASS pModel = g_pLTServer->CreateObject( hClass, &theStruct );
	if( !pModel ) return false;

	LTRotation rRot( 0.0f, MATH_PI, 0.0f );
	LTVector vOffset( 0.0f, 0.0f, 0.0f );
	HATTACHMENT hAttachment;
	if( g_pLTServer->CreateAttachment( m_hObject, pModel->m_hObject, LTNULL, &vOffset, &rRot, &hAttachment ) != LT_OK )
	{
		g_pLTServer->RemoveObject( pModel->m_hObject );
		return false;
	}

	if( !m_Weapons.Init( m_hObject, pModel->m_hObject ))
	{
		g_pLTServer->RemoveAttachment( hAttachment );
		g_pLTServer->RemoveObject( pModel->m_hObject );
		return false;
	}

	m_Weapons.ObtainWeapon( nWeaponId, nAmmoId, 10000 );
	m_Weapons.ChangeWeapon( nWeaponId );

	CWeapon *pCurWeapon = m_Weapons.GetCurWeapon( );
	if( pCurWeapon )
	{
		pCurWeapon->SetAmmoId( nAmmoId );
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::GetFlashPosition
//
//	PURPOSE:	RETAIL: Object 0x100e5090: the weapon model's Flash socket
//
// ----------------------------------------------------------------------- //

bool GunMount::GetFlashPosition( LTVector& vPos )
{
	CWeapon *pWeapon = m_Weapons.GetCurWeapon( );
	if( !pWeapon ) return false;

	HATTACHMENT hAttachment;
	if( g_pLTServer->FindAttachment( m_hObject, pWeapon->GetModelObject( ), &hAttachment ) != LT_OK )
		return false;

	HMODELSOCKET hSocket;
	if( g_pModelLT->GetSocket( pWeapon->GetModelObject( ), "Flash", hSocket ) != LT_OK )
		return false;

	LTransform transform;
	g_pCommonLT->GetAttachedModelSocketTransform( hAttachment, hSocket, transform );

	vPos = transform.m_Pos;
	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::FireWeapon
//
//	PURPOSE:	RETAIL: Object 0x100e53a0: one shot; the ammo never runs out
//
// ----------------------------------------------------------------------- //

bool GunMount::FireWeapon( )
{
	CWeapon *pWeapon = m_Weapons.GetCurWeapon( );
	if( !pWeapon ) return false;

	LTRotation rRot;
	g_pLTServer->GetObjectRotation( m_hObject, &rRot );

	// RETAIL: Object 0x100e540b: the forward is taken before the flash
	// position.
	LTVector vForward = rRot.Forward( );

	LTVector vFlashPos;
	if( !GetFlashPosition( vFlashPos ))
		return false;

	WeaponFireInfo weaponFireInfo;
	weaponFireInfo.hFiredFrom	= m_hObject;
	weaponFireInfo.vPath		= vForward;
	weaponFireInfo.vFirePos		= vFlashPos;
	weaponFireInfo.vFlashPos	= vFlashPos;

	if( pWeapon->UpdateWeapon( weaponFireInfo, LTTRUE ) == W_FIRED )
	{
		if( m_nRoundsToFire > 0 )
		{
			m_nRoundsToFire--;
		}
	}

	m_Weapons.AddAmmo( pWeapon->GetAmmoId( ), 10000 );
	pWeapon->ReloadClip( LTFALSE );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::SetVisible
//
//	PURPOSE:	RETAIL: Object 0x100e4d10
//
// ----------------------------------------------------------------------- //

void GunMount::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;

	CWeapon *pWeapon = m_Weapons.GetCurWeapon( );
	if( !pWeapon ) return;

	g_pCommonLT->SetObjectFlags( pWeapon->GetModelObject( ), OFT_Flags,
		( m_bVisible ? FLAG_VISIBLE : 0 ) | FLAG_FORCECLIENTUPDATE, FLAG_VISIBLE | FLAG_FORCECLIENTUPDATE );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::IsAnimationDone
//
//	PURPOSE:	RETAIL: Object 0x100e4c00
//
// ----------------------------------------------------------------------- //

bool GunMount::IsAnimationDone( )
{
	CWeapon *pWeapon = m_Weapons.GetCurWeapon( );
	if( !pWeapon ) return true;

	uint32 dwState = 0;
	if( g_pModelLT->GetPlaybackState( pWeapon->GetModelObject( ), MAIN_TRACKER, dwState ) != LT_OK )
		return true;

	// RETAIL: Object 0x100e4c54: the flag's bit converts to the bool.
	return ( dwState & MS_PLAYDONE ) ? true : false;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::BeginFire
//
//	PURPOSE:	RETAIL: Object 0x100e56d0
//
// ----------------------------------------------------------------------- //

bool GunMount::BeginFire( )
{
	CWeapon *pWeapon = m_Weapons.GetCurWeapon( );
	if( !pWeapon ) return false;

	HMODELANIM hAni = pWeapon->GetPreFireAni( );
	if( hAni == INVALID_MODEL_ANIM )
		return StartFiring( );

	pWeapon->PlayAnimation( hAni, true, false, true );
	SetNextUpdate( UPDATE_NEXT_FRAME );
	m_eFiringState = ePreFiring;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::StartFiring
//
//	PURPOSE:	RETAIL: Object 0x100e5620
//
// ----------------------------------------------------------------------- //

bool GunMount::StartFiring( )
{
	CWeapon *pWeapon = m_Weapons.GetCurWeapon( );
	if( !pWeapon ) return false;

	HMODELANIM hAni = pWeapon->GetFireAni( );
	if( hAni != INVALID_MODEL_ANIM )
	{
		pWeapon->PlayAnimation( hAni, true, true, false );
	}

	if( !FireWeapon( ))
	{
		m_eFiringState = eNotFiring;
		return true;
	}

	m_eFiringState = eFiring;
	SetNextUpdate( UPDATE_NEXT_FRAME );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::EndFire
//
//	PURPOSE:	RETAIL: Object 0x100e4c70
//
// ----------------------------------------------------------------------- //

bool GunMount::EndFire( )
{
	CWeapon *pWeapon = m_Weapons.GetCurWeapon( );
	if( !pWeapon ) return false;

	HMODELANIM hAni = pWeapon->GetPostFireAni( );
	if( hAni == INVALID_MODEL_ANIM )
	{
		m_eFiringState = eNotFiring;
		return true;
	}

	pWeapon->PlayAnimation( hAni, true, false, true );
	m_eFiringState = ePostFiring;
	SetNextUpdate( UPDATE_NEXT_FRAME );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::UpdatePreFiring
//
//	PURPOSE:	RETAIL: Object 0x100e5740
//
// ----------------------------------------------------------------------- //

void GunMount::UpdatePreFiring( )
{
	if( IsAnimationDone( ))
	{
		StartFiring( );
	}
	else
	{
		SetNextUpdate( UPDATE_NEXT_FRAME );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::UpdateFiring
//
//	PURPOSE:	RETAIL: Object 0x100e56a0
//
// ----------------------------------------------------------------------- //

void GunMount::UpdateFiring( )
{
	if( m_nRoundsToFire != 0 && FireWeapon( ))
	{
		SetNextUpdate( UPDATE_NEXT_FRAME );
	}
	else
	{
		EndFire( );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::UpdatePostFiring
//
//	PURPOSE:	RETAIL: Object 0x100e4ce0
//
// ----------------------------------------------------------------------- //

void GunMount::UpdatePostFiring( )
{
	if( IsAnimationDone( ))
	{
		m_eFiringState = eNotFiring;
	}
	else
	{
		SetNextUpdate( UPDATE_NEXT_FRAME );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::Update
//
//	PURPOSE:	RETAIL: Object 0x100e5b80
//
// ----------------------------------------------------------------------- //

void GunMount::Update( )
{
	switch( m_eFiringState )
	{
		case ePreFiring:	UpdatePreFiring( );		break;
		case eFiring:		UpdateFiring( );		break;
		case ePostFiring:	UpdatePostFiring( );	break;
		default : break;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::OnTrigger
//
//	PURPOSE:	RETAIL: Object 0x100e5850
//
// ----------------------------------------------------------------------- //

bool GunMount::OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg )
{
	static CParsedMsg::CToken s_cTok_1( "1" );
	static CParsedMsg::CToken s_cTok_True( "TRUE" );
	static CParsedMsg::CToken s_cTok_Fire( "FIRE" );
	static CParsedMsg::CToken s_cTok_BeginFire( "BEGINFIRE" );
	static CParsedMsg::CToken s_cTok_EndFire( "ENDFIRE" );
	static CParsedMsg::CToken s_cTok_Visible( "VISIBLE" );
	static CParsedMsg::CToken s_cTok_Hidden( "HIDDEN" );
	static CParsedMsg::CToken s_cTok_Remove( "REMOVE" );

	if( cMsg.GetArg(0) == s_cTok_Fire )
	{
		m_nRoundsToFire = ( cMsg.GetArgCount( ) > 1 ) ? atoi( cMsg.GetArg(1) ) : 1;
		BeginFire( );
	}
	else if( cMsg.GetArg(0) == s_cTok_BeginFire )
	{
		m_nRoundsToFire = -1;
		BeginFire( );
	}
	else if( cMsg.GetArg(0) == s_cTok_EndFire )
	{
		EndFire( );
	}
	else if( cMsg.GetArg(0) == s_cTok_Visible )
	{
		bool bOn = ( cMsg.GetArg(1) == s_cTok_1 || cMsg.GetArg(1) == s_cTok_True );
		SetVisible( bOn );
	}
	else if( cMsg.GetArg(0) == s_cTok_Hidden )
	{
		bool bOn = ( cMsg.GetArg(1) == s_cTok_1 || cMsg.GetArg(1) == s_cTok_True );
		SetVisible( !bOn );
	}
	else
	{
		return GameBase::OnTrigger( hSender, cMsg );
	}

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::Save
//
//	PURPOSE:	RETAIL: Object 0x100e4dd0
//
// ----------------------------------------------------------------------- //

void GunMount::Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags )
{
	if( !pMsg ) return;

	pMsg->WriteString( m_sWeapon.c_str( ));
	m_Weapons.Save( pMsg, 0 );
	SAVE_INT( m_nRoundsToFire );
	SAVE_BYTE( m_eFiringState );
	SAVE_bool( m_bVisible );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GunMount::Load
//
//	PURPOSE:	RETAIL: Object 0x100e5770
//
// ----------------------------------------------------------------------- //

void GunMount::Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags )
{
	if( !pMsg ) return;

	char szWeapon[256];
	pMsg->ReadString( szWeapon, ARRAY_LEN( szWeapon ));
	m_sWeapon = szWeapon;

	m_Weapons.Load( pMsg, 0 );

	int nCurWeapon = m_Weapons.GetCurWeaponId( );
	if( nCurWeapon >= 0 )
	{
		m_Weapons.DeselectCurWeapon( );
		m_Weapons.ChangeWeapon( nCurWeapon );
	}

	LOAD_INT( m_nRoundsToFire );
	LOAD_BYTE( m_eFiringState );
	LOAD_bool( m_bVisible );

	switch( m_eFiringState )
	{
		case ePreFiring:
		case eFiring:
			BeginFire( );
		break;

		case eNotFiring:
		case ePostFiring:
		default:
			m_eFiringState = eNotFiring;
		break;
	}
}
