/*
 * clientpad.h -- the modern build's controller, the client's half
 * (MODERN.md CL-20 .. CL-23). The engine's ##gamepad device
 * (gamepad_device.h, EN-20) gives the pad's buttons to the bindings like
 * any key's; this reads the pad itself for what must not depend on them:
 * the sticks (analog look and movement, the snowmobile), the keys the menus
 * and screens take, A in a cut-scene and on the death screen, and the
 * choices of a dialogue decision. The retail files call into these from
 * CJACK_MODERN blocks; clientpad.cpp, the implementation, is compiled as
 * part of GameClientShell.cpp.
 *
 * Modern build only (CJACK_MODERN).
 */
#ifndef CJACK_CLIENTPAD_H
#define CJACK_CLIENTPAD_H

class CUserProfile;

// CGameClientShell::Update, every frame, menus too: read the pad, and the
// menus' keys and A's
void	Pad_Update(float fFrameTime);

// CGameClientShell::OnEngineTerm: stop looking for a pad (the thread that
// does must end before the DLL is unloaded)
void	Pad_Term();

// CL-20: the right stick, as this frame's turn (yaw) and look (pitch) in
// radians; 0 outside play
void	Pad_GetLook(float* pYaw, float* pPitch);

// CL-20: the left stick: the movement flags by its direction, and running
// past three-quarters (bCanRun: not ducking or leaning)
void	Pad_MoveFlags(uint32* pdwFlags, bool bCanRun);

// CL-20: the left stick past its dead zone, -1..1 (forward positive); false
// when it is not pushed
bool	Pad_GetMove(float* pRight, float* pForward);

// CL-20: the share of the walking speed the stick asks for (1 when it is
// not pushed, or pushed to run)
float	Pad_SpeedScale();

// CL-20: on the snowmobile, the stick's throttle, brake and steering
void	Pad_VehicleFlags(uint32* pdwFlags);

// CL-22: Configure controls is waiting for a control to bind; the pad's
// menu keys stop meanwhile
void	Pad_SetCapture(bool bCapture);

// CL-21: give the profile's controller column the default layout, leaving
// a command the player has bound on the controller, and a button already
// bound, alone (bReplace: the column cleared first, Restore controller
// defaults)
void	Pad_DefaultBindings(CUserProfile* pProfile, bool bReplace);

#endif // CJACK_CLIENTPAD_H
