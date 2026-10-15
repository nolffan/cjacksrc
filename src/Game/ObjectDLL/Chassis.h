// ----------------------------------------------------------------------- //
//
// MODULE  : Chassis.h
//
// PURPOSE : A prop that chassis pieces are carried to and put on.  Each
//           piece it allows has a slot, shown by a see-through model until
//           a piece fills it; when every slot is filled it runs its
//           OnComplete command.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from Object.lto
// (0x100abf80..0x100ade30).  It is NOLF2's DoomsDayDevice with the
// DoomsDay parts taken out, which DoomsDayDevice now derives from.
//
// ----------------------------------------------------------------------- //

#ifndef __CHASSIS_H__
#define __CHASSIS_H__

#include "PropType.h"
#include "CommandMgr.h"

class CPlayerObj;
class ChassisPiece;
struct CHASSISPIECE;

LINKTO_MODULE( Chassis );

class Chassis : public PropType
{
	public: // Methods...

		Chassis( );
		~Chassis( );

		uint8	GetTeamID( )			const { return m_nOwningTeamID; }
		float	GetDropZoneRadius( )	const { return m_fDropZoneRadius; }

		// Are all the slots filled?
		bool	IsComplete( ) const;

		// Put a piece in its slot, or take it out again.
		virtual bool	AddPiece( ChassisPiece *pPiece, CPlayerObj *pPlayer );
		virtual bool	RemovePiece( ChassisPiece *pPiece, CPlayerObj *pPlayer );

		typedef std::vector< Chassis* > ChassisList;
		static ChassisList const& GetChassisList( ) { return m_lstChassis; }

	protected: // Methods...

		uint32	EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData );

		bool	OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg );

	protected: // Members...

		// One piece the chassis takes: the see-through model showing where
		// it goes, and the piece once it is there.
		struct Slot
		{
			Slot( ) { m_pType = LTNULL; }

			CHASSISPIECE const*	m_pType;
			LTObjRef			m_hModel;
			LTObjRef			m_hPiece;
		};

		typedef std::vector< Slot > SlotList;

		// Team that owns this chassis.
		uint8			m_nOwningTeamID;

		// Radius of the area considered the 'drop zone'.
		// When a piece is dropped within this radius it will be added to the chassis.
		float			m_fDropZoneRadius;

		// Command run when the last piece goes on.
		std::string		m_sOnComplete;

		SlotList		m_Slots;

	private: // Methods...

		bool	ReadProp( ObjectCreateStruct* pStruct );
		void	CreateSlotModels( );
		Slot*	FindSlot( CHASSISPIECE const* pType );
		void	ClearSlots( );
		void	CreateSFXMessage( );

		void	Save( ILTMessage_Write *pMsg, uint32 dwSaveFlags );
		void	Load( ILTMessage_Read *pMsg, uint32 dwLoadFlags );

	private: // Members...

		// List of all the chassis within the game.
		static ChassisList	m_lstChassis;
};

class CChassisPlugin : public CPropTypePlugin
{
	public:

		virtual LTRESULT PreHook_EditStringList(
			const char *szRezPath,
			const char *szPropName,
			char **aszStrings,
			uint32 *pcStrings,
			const uint32 cMaxStrings,
			const uint32 cMaxStringLen );

		virtual LTRESULT PreHook_PropChanged(
			const char *szObjName,
			const char *szPropName,
			const int nPropType,
			const GenericProp &gpPropValue,
			ILTPreInterface *pInterface,
			const char *szModifiers );

	protected:

		CCommandMgrPlugin	m_CommandMgrPlugin;
};

#endif // __CHASSIS_H__
