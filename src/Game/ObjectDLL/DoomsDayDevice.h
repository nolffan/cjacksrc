// ----------------------------------------------------------------------- //
//
// MODULE  : DoomsDayDevice.h
//
// PURPOSE : The object used to represent the doomsday device.
//
// CREATED : 12/19/02
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __DOOMS_DAY_DEVICE_H__
#define __DOOMS_DAY_DEVICE_H__

//
// Includes...
//

#include "Chassis.h"
#include "Timer.h"

class CPlayerObj;

LINKTO_MODULE( DoomsDayDevice );

// cjacksrc: Contract J.A.C.K. makes the device a Chassis that takes the
// three DoomsDay pieces (Object.lto 0x100c7690..0x100c8240).
class DoomsDayDevice : public Chassis
{
	public: // Methods...

		DoomsDayDevice( );
		~DoomsDayDevice( );

		// Fires the device.
		bool	Fire( CPlayerObj& playerObj );

		bool	AddPiece( ChassisPiece *pPiece, CPlayerObj *pPlayer );
		bool	RemovePiece( ChassisPiece *pPiece, CPlayerObj *pPlayer );

		typedef std::vector< DoomsDayDevice* > DoomsDayDeviceList;
		static DoomsDayDeviceList const& GetDoomsDayDeviceList( ) { return m_lstDoomsDayDevices; }

	protected: // Methods...

		uint32	EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData );

		bool	OnTrigger( HOBJECT hSender, const CParsedMsg &cMsg );

	private:

		void	InitialUpdate( );
		void	Update( );

		bool	BeginIdle( );
		void	UpdateIdle( );

		bool	BeginFire( );
		void	UpdateFire( );

		bool	BeginEnd( );
		void	UpdateEnd( );


	private:

		void	Save(ILTMessage_Write *pMsg, uint32 dwSaveFlags);
		void	Load(ILTMessage_Read *pMsg, uint32 dwLoadFlags);

		// Called to give damage to victim teams.
		bool	DamageVictims( bool& bAllDead );

		enum DoomsDayDeviceState
		{
			kDoomsDayDeviceState_Idle,
			kDoomsDayDeviceState_Fire,
			kDoomsDayDeviceState_End,
		};

		// Current state of the device.
		DoomsDayDeviceState		m_eDoomsDayDeviceState;

		// Time to stay in fire state.
		CTimer					m_Timer;

		// List of all DoomsDayDevices within the game.
		static DoomsDayDeviceList	m_lstDoomsDayDevices;
};

class CDoomsDayDevicePlugin : public CChassisPlugin
{
};

#endif // __DOOMS_DAY_DEVICE_H__
