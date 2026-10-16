// ----------------------------------------------------------------------- //
//
// MODULE  : VehicleWeapon.h
//
// PURPOSE : The weapon of a vehicle the player drives.  The vehicle's
//           model is its model; its fire animations change as the vehicle
//           drives at full speed or brakes.
//
// cjacksrc: Contract J.A.C.K.'s own; reconstructed from CShell.dll
// (0x1010c810..0x1010cf45).
//
// ----------------------------------------------------------------------- //

#ifndef __VEHICLE_WEAPON_H__
#define __VEHICLE_WEAPON_H__

#include "ClientWeapon.h"

class CVehicleWeapon : public CClientWeapon
{
public:

	CVehicleWeapon();
	virtual ~CVehicleWeapon();

	// The vehicle model's parts the weapon has nothing of.
	virtual void          ResetWeaponFilenames() {}
	virtual void          UpdateBob( LTFLOAT fWidth, LTFLOAT fHeight ) {}
	virtual void          SetVisible( bool bVis = true ) {}
	virtual void          SetDisable( bool bDisable = true ) { m_bDisabled = bDisable; }

	virtual void          Select() {}
	virtual bool          Deselect( ClientWeaponCallBackFn cbFn, void *pData ) { return false; }
	virtual bool          Activate();
	virtual void          Deactivate();

	// The vehicle's model is the weapon's.
	virtual void          SetVehicleModel( HOBJECT hModel );
	virtual void          SetFullSpeed( bool bFullSpeed ) { m_bFullSpeed = bFullSpeed; }
	virtual void          SetBraking( bool bBraking ) { m_bBraking = bBraking; }

protected:

	virtual bool          CreateWeaponModel() { return false; }
	virtual void          RemoveWeaponModel() { m_hObject = LTNULL; }

	virtual void          InitAnimations( bool bAllowSelectOverride = false );

	virtual bool          PlaySelectAnimation() { return false; }
	virtual bool          PlayDeselectAnimation() { return false; }
	virtual bool          PlayReloadAnimation() { return false; }
	virtual bool          PlayIdleAnimation() { return false; }
	virtual bool          PlayFireAnimation( bool bResetAni );

	virtual bool          IsPreFireAni( uint32 dwAni ) const;
	virtual uint32        GetPreFireAni() const;
	virtual bool          IsFireAni( uint32 dwAni , bool bCheckNormalOnly = false) const;
	virtual uint32        GetFireAni( FireType eFireType=FT_NORMAL_FIRE ) const;
	virtual bool          IsPostFireAni( uint32 dwAni ) const;
	virtual uint32        GetPostFireAni() const;

	bool          m_bFullSpeed;
	HMODELANIM    m_nFullSpeedFireAnis[WM_MAX_FIRE_ANIS];
	HMODELANIM    m_nFullSpeedPreFireAni;
	HMODELANIM    m_nFullSpeedPostFireAni;

	bool          m_bBraking;
	HMODELANIM    m_nBrakeFireAnis[WM_MAX_FIRE_ANIS];
	HMODELANIM    m_nBrakePreFireAni;
	HMODELANIM    m_nBrakePostFireAni;
};

#endif // __VEHICLE_WEAPON_H__
