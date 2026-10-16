// ----------------------------------------------------------------------- //
//
// MODULE  : VehicleButeMgr.h
//
// PURPOSE : The vehicles of attributes\VehicleButes.txt: their models,
//           sounds, handling and weapon.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from Object.lto
// (0x1015bae0..0x1015c9c9) and CShell.dll, which both have it.
//
// ----------------------------------------------------------------------- //

#ifndef __VEHICLE_BUTE_MGR_H__
#define __VEHICLE_BUTE_MGR_H__

#include "GameButeMgr.h"
#include "ButeListReader.h"

#define VBMGR_DEFAULT_FILE		"Attributes\\VehicleButes.txt"
#define VBMGR_INVALID_ID		0xFF

class CVehicleButeMgr;
extern CVehicleButeMgr* g_pVehicleButeMgr;

// One Vehicle<n> record (0x264 bytes).
struct VEHICLEBUTES
{
	VEHICLEBUTES();
	~VEHICLEBUTES();

	bool	Read( CButeMgr &buteMgr, char const* pszTag );
	void	Save( CButeMgr &buteMgr );

	uint8			nId;
	std::string		sName;
	std::string		sModel;
	CButeListReader	blrSkins;
	CButeListReader	blrRenderStyles;
	bool			bTranslucent;
	std::string		sPVModel;
	CButeListReader	blrPVSkins;
	CButeListReader	blrPVRenderStyles;

	std::string		sStartUpSnd;
	std::string		sIdleSnd;
	std::string		sAccelSnd;
	std::string		sDecelSnd;
	std::string		sBrakeSnd;
	std::string		sTurnOffSnd;
	std::string		sSlowImpactSnd;
	std::string		sMedImpactSnd;
	std::string		sFastImpactSnd;
	std::string		sMPLoop;

	float			fVelocity;
	float			fAcceleration;
	float			fAccelerationTime;
	float			fMaxDeceleration;
	float			fDecelerationTime;
	float			fMaxBrake;
	float			fBrakeTime;
	float			fStoppedPercent;
	float			fMaxSpeedPercent;
	float			fMinTurnAccel;
	float			fTurnRate;
	float			fInitialTurnRate;
	float			fSurfaceSpeedUpPitch;
	float			fSurfaceSlowDownPitch;
	float			fStartMotionPitch;
	float			fStopMotionPitch;
	float			fBrakePitch;
	float			fDecelPitch;
	float			fImpactAngleSensitivity;
	LTVector		vPVOffset;

	std::string		sVehicleWeapon;
	bool			bCanShowCrosshair;
	bool			bContourAffectsRoll;
	uint8			nSurfaceVehicleType;
	bool			bHasReverse;
	bool			bMotorcycle;

	std::string		sMPFadeFX;
	std::string		sMPSpawnFX;
	std::string		sMPUnspawnFX;
};

class CVehicleButeMgr : public CGameButeMgr
{
	private :

		// Not allowed to create directly.  Use Instance().
		CVehicleButeMgr();

		// Copy ctor and assignment operator not implemented and should never be used.
		CVehicleButeMgr( CVehicleButeMgr const& other );
		CVehicleButeMgr& operator=( CVehicleButeMgr const& other );

	public :

		virtual ~CVehicleButeMgr();

		// Call this to get the singleton instance of the vehicle mgr.
		static CVehicleButeMgr& Instance( );

		virtual LTBOOL	Init( const char* szAttributeFile = VBMGR_DEFAULT_FILE );
		virtual void	Term( );

		// Write the vehicles' tweakable values back to the file, one
		// vehicle's or all of them.
		bool			WriteVehicleButes( VEHICLEBUTES* pVehicle = LTNULL );

		uint32				GetNumVehicles( ) const { return m_VehicleList.size( ); }
		VEHICLEBUTES*		GetVehicle( uint8 nId ) const;
		VEHICLEBUTES*		GetVehicle( char const* pszName );

	private :

		typedef std::vector< VEHICLEBUTES* > VehicleList;
		VehicleList		m_VehicleList;
};

#endif // __VEHICLE_BUTE_MGR_H__
