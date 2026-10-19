// ----------------------------------------------------------------------- //
//
// MODULE  : GunMount.h
//
// PURPOSE : GunMount - Definition
//
// CREATED : RETAIL: Contract J.A.C.K.'s (Object 0x100e4b70..0x100e5d90)
//
// ----------------------------------------------------------------------- //

#ifndef __GUN_MOUNT_H__
#define __GUN_MOUNT_H__

#include "ltengineobjects.h"
#include "GameBase.h"
#include "Weapons.h"
#include "iobjectplugin.h"

#pragma warning( disable : 4786 )
#include <string>

LINKTO_MODULE( GunMount );

// A weapon fixed in the level that fires when told to: FIRE [rounds],
// BEGINFIRE and ENDFIRE.

class GunMount : public GameBase
{
	public :

		GunMount();
		~GunMount();

	protected :

		uint32			EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData );
		uint32			ObjectMessageFn( HOBJECT hSender, ILTMessage_Read *pMsg );
		bool			OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg );

	private :

		enum FiringState
		{
			eNotFiring,
			ePreFiring,
			eFiring,
			ePostFiring,
		};

		LTBOOL			ReadProp( ObjectCreateStruct *pStruct );
		void			InitialUpdate( );
		void			Update( );

		bool			CreateWeapon( );
		bool			GetFlashPosition( LTVector& vPos );
		bool			FireWeapon( );
		void			SetVisible( bool bVisible );

		bool			IsAnimationDone( );
		bool			BeginFire( );
		bool			StartFiring( );
		bool			EndFire( );

		void			UpdatePreFiring( );
		void			UpdateFiring( );
		void			UpdatePostFiring( );

		void			Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags );
		void			Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags );

		std::string		m_sWeapon;
		CWeapons		m_Weapons;
		int				m_eFiringState;
		int				m_nRoundsToFire;	// -1 until ENDFIRE
		bool			m_bVisible;
};

class GunMountPlugin : public IObjectPlugin
{
	public :

		virtual LTRESULT PreHook_EditStringList(
			const char* szRezPath,
			const char* szPropName,
			char** aszStrings,
			uint32* pcStrings,
			const uint32 cMaxStrings,
			const uint32 cMaxStringLength );

		virtual LTRESULT PreHook_Dims(
			const char* szRezPath,
			const char* szPropValue,
			char* szModelFilenameBuf,
			int	  nModelFilenameBufLen,
			LTVector & vDims );
};

#endif // __GUN_MOUNT_H__
