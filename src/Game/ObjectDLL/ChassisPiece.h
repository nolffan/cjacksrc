// ----------------------------------------------------------------------- //
//
// MODULE  : ChassisPiece.h
//
// PURPOSE : A piece a player carries to a Chassis and puts on it.  Its
//           type is a ChassisPiece record of attributes\chassis.txt.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from Object.lto
// (0x100ae720..0x100af950).  It is NOLF2's DoomsDayPiece with the
// DoomsDay parts taken out, which DoomsDayPiece now derives from.
//
// ----------------------------------------------------------------------- //

#ifndef __CHASSIS_PIECE_H__
#define __CHASSIS_PIECE_H__

//
// Includes...
//

#include "PropType.h"
#include "Timer.h"

struct CHASSISPIECE;

LINKTO_MODULE( ChassisPiece );

class ChassisPiece : public PropType
{
	public: // Methods...

		ChassisPiece( );
		~ChassisPiece( );

		CHASSISPIECE const*	GetChassisPiece( ) const { return m_pChassisPiece; }
		bool	IsHeavy( ) const;

	protected: // Methods...

		uint32	EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData );

		void	SetNextUpdate( LTFLOAT fDelta, eUpdateControl eControl = eControlDeactivation );

		bool	OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg );

		// Message handlers...

		virtual bool	OnCarry( HOBJECT hSender );
		virtual bool	OnDrop( HOBJECT hSender );

		// Put the piece back where it started.
		virtual bool	Respawn( );

		void	CreateSFXMessage( bool bCarried, uint8 nTeam, bool bSendToClients );

	private: // Methods...

		bool	ReadProp( ObjectCreateStruct *pOCS );

		void	Update( );

		void	Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags );
		void	Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags );

	protected:	// Members...

		CHASSISPIECE const*	m_pChassisPiece;

		LTObjRef		m_hChassis;			// hObject of the chassis we are a part of.

		CTimer			m_RespawnTimer;		// Countdown timer for respawning after being dropped.
		float			m_fRespawnTime;		// How long a dropped piece lies before it respawns.
		bool			m_bRespawnSoon;		// The respawn timer is in its last seconds.
		CTimer			m_DelayTimer;		// Countdown timer to delay picking up the piece after it drops

		LTVector		m_vOriginalPos;		// Position the piece was originally placed.

		bool			m_bCanBeCarried;	// Can this piece currently be picked up.
};

class CChassisPiecePlugin : public CPropTypePlugin
{
	public:

		virtual LTRESULT PreHook_EditStringList(
			const char *szRezPath,
			const char *szPropName,
			char **aszStrings,
			uint32 *pcStrings,
			const uint32 cMaxStrings,
			const uint32 cMaxStringLen );

		virtual LTRESULT PreHook_Dims(
			const char *szRezPath,
			const char *szPropValue,
			char *szModelFilenameBuf,
			int nModelFilenameBufLen,
			LTVector &vDims );
};

#endif // __CHASSIS_PIECE_H__
