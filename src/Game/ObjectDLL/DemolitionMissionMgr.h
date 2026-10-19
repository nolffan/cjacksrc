// ----------------------------------------------------------------------- //
//
// MODULE  : DemolitionMissionMgr.h
//
// PURPOSE : Contract J.A.C.K.'s Demolition game type (RETAIL: Object
//           0x100c09a0..0x100c0d50, vtable 0x101bc730).
//
// ----------------------------------------------------------------------- //

#ifndef __DEMOLITIONMISSIONMGR_H__
#define __DEMOLITIONMISSIONMGR_H__

#include "ServerMissionMgr.h"

// Two teams, an even number of rounds (each team attacks as often as it
// defends), no score or time limit of its own: the worlds' bomb targets
// end the rounds.

class CDemolitionMissionMgr : public CServerMissionMgr
{
	public:

		CDemolitionMissionMgr();
		~CDemolitionMissionMgr();

	public:

		virtual bool	Init( );
		virtual void	Update( );

	protected:

		virtual bool	StartGame( ILTMessage_Read& msg );
		virtual bool	EndGame( );
		virtual bool	FinishExitLevel( );
		virtual void	LevelStarted( );
		virtual bool	HandleMultiplayerOptions( HCLIENT hSender, ILTMessage_Read& msg );
};

#endif // __DEMOLITIONMISSIONMGR_H__
