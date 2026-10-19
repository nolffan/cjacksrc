// ----------------------------------------------------------------------- //
//
// MODULE  : AimMagnet.h
//
// PURPOSE : AimMagnet - Definition
//
// CREATED : RETAIL: Contract J.A.C.K.'s (Object 0x10059520..0x1005a240)
//
// ----------------------------------------------------------------------- //

#ifndef __AIM_MAGNET_H__
#define __AIM_MAGNET_H__

#include "ltengineobjects.h"
#include "GameBase.h"
#include "iobjectplugin.h"

#pragma warning( disable : 4786 )
#include <vector>
#include <string>
#include <algorithm>

LINKTO_MODULE( AimMagnet );

// An object heat-seeking projectiles steer toward (and the client's
// auto-aim pulls to), optionally only for one team's shots; it can follow
// another object (Target).

class AimMagnet : public GameBase
{
	public :

		AimMagnet();
		~AimMagnet();

		uint8			GetTeamId() const { return m_nTeamId; }
		HOBJECT			GetTarget() const { return m_hTarget; }

		typedef std::vector< AimMagnet* > AimMagnetList;
		static AimMagnetList const& GetAimMagnetList( ) { return m_lstAimMagnets; }

	protected :

		uint32			EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData );
		bool			OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg );

		virtual void	CreateSpecialFX( bool bUpdateClients );

		void			OnLinkBroken( LTObjRefNotifier *pRef, HOBJECT hObj );

	private :

		LTBOOL			ReadProp( ObjectCreateStruct *pStruct );
		LTBOOL			InitialUpdate( );
		LTBOOL			Update( );
		void			SetTeam( uint8 nTeamId );
		void			ResolveTarget( );

		void			Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags );
		void			Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags );

		std::string			m_sTargetName;
		LTObjRefNotifier	m_hTarget;
		uint8				m_nTeamId;

		static AimMagnetList	m_lstAimMagnets;
};

class CAimMagnetPlugin : public IObjectPlugin
{
	public :

		virtual LTRESULT PreHook_EditStringList(
			const char* szRezPath,
			const char* szPropName,
			char** aszStrings,
			uint32* pcStrings,
			const uint32 cMaxStrings,
			const uint32 cMaxStringLength );
};

#endif // __AIM_MAGNET_H__
