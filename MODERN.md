# The modern build

`CJACK_MODERN` is the project's define for everything that departs from the
retail game: bug fixes (`RETAIL_BUGS.md`), new features, and open-source
middleware in place of RAD's. It is set only in the modern build (`make
modern`, `make modern-debug`; `build/variant.sh`), never in the legacy
build, which stays the retail game as far as it can be rebuilt and is the
only build `make progress` scores. Every change goes behind
`#ifdef CJACK_MODERN` with the retail code kept, unchanged, in the `#else`.

This file is the inventory: what the modern build changes, what it could
change next, and what was looked at and left alone, and why. It follows
nolf2src's `MODERN.md` (NOLF2's modern build) in its rules; the items are
Contract J.A.C.K.'s own.

Each item's id is a series and a number (GS-1, GAME-3, EN-1, APPS-4, CL-1,
LAY-1); an id is never reused.

## Rules for this work

* **Reimplement, never copy.** Fan fixes and patches are a fine source for
  *what* is wrong and *why*; their code is not copied here. The same goes
  for nolf2src's NOLF2 fixes: the idea often carries over, since the code is
  the same generation, but each change is made and checked here.
* **The 2003 compiler.** The modern build uses the legacy build's toolchain
  (Visual C++ .NET 2003, the DirectX 8.1 SDK).
* **No SecuROM, no disc check.** The engine built here has no SecuROM; the
  modern build also drops the game's own disc and CD-key checks.

## Planned, from nolf2src's modern build

To bring over once the game runs (each to be checked against Contract
J.A.C.K.'s code first): mouse look without the retail input's repeated last
event; a frame limiter; our Bink player on FFmpeg in place of RAD's
`binkw32.dll`; Internet play without the dead master servers. (Widescreen
menus, HUD and field of view and the window modes are done: EN-30 to EN-34,
CL-30 to CL-35, APPS-30 to APPS-32.)

Controller follow-ups (nolf2src's M-G64, M-G65; nolfsrc's M-P7-07, M-P7-08):

* **Rumble.** Vibration on firing, damage and explosions near by, through
  XInput's `XInputSetState`. Nothing in the game asks for it
  (`PlayJoystickEffect` is commented out in the engine), so it needs its
  own calls from the client; the ##gamepad device (EN-20) answers
  DirectInput's force-feedback calls with "unsupported".
* **Button prompts.** Hints that name a key ("Press [key]") would name the
  controller's button when the controller was used last.

## New console variables

Client-side console variables the modern build adds. Creating one does not
save it: `autoexec.cfg` keeps those set with a `+` (as the options screens
set theirs) and those set as `+Name value` on the command line;
`display.cfg` (written by the game at every start, read by the launcher)
keeps the display's. The modern build's strings are IDs 31000 and up
(`ClientResShared.h`, the English `ClientResShared.rc`): the Display
screen's 31000-31011 and 31200-31202, the controller's 31100-31114; the
launcher's are in its own `resource.h`.

| Variable | Type | Default | Meaning | Item |
|---|---|---|---|---|
| `PadLookSpeed` | float | 1 | the controller's right stick: its turn rate, times 180 degrees a second across and 120 up and down; Controller's "Look speed" (25-300%) | CL-20, CL-23 |
| `PadInvertY` | int | 0 | the right stick's up looks down; Controller's "Invert look" | CL-20, CL-23 |
| `PadLookCurve` | float | 2 | the right stick's response: its deflection past the dead zone, raised to this (1 = linear; less than 1 is taken as 1) | CL-20 |
| `PadLookDeadZone` | float | 0.15 | the right stick's radial dead zone (0..0.9 of its travel); Controller's "Look dead zone" (0-50%) | CL-20, CL-23 |
| `PadMoveDeadZone` | float | 0.2 | the left stick's radial dead zone (0..0.9); Controller's "Move dead zone" (0-50%) | CL-20, CL-23 |
| `PadHoldButtons` | int | 0 | the controller's buttons (XInput's bits) the client reads itself for now and the bindings do not see; set by the game every time it changes, never saved; not a setting | EN-20, CL-22 |
| `Windowed` | int | (engine) | existing (0 full screen, 1 a window: what the renderer reads); now set by the Display screen's and the launcher's "Window mode", applied at once, and kept in `display.cfg`; a fresh install's is the launcher's default (a window; full screen on a Steam Deck) | CL-32, APPS-31, APPS-32 |
| `WindowMode` | int | 0 | with `Windowed` 1: 1 a framed window, 2 a borderless window over the whole desktop at the desktop's resolution; 0 (a config from before it) borderless only when the mode is as large as the desktop | EN-32, CL-32, APPS-31 |
| `Cinematics43` | int | 0 | on a wide screen, cutscenes in the centred 4:3 area; Display's "4:3 cinematics" | CL-35 |

## Done

* **GS-1: GameSpy.dll's handles.** Each request thread's start event no
  longer leaks the event its base class made, and the server browser's
  constructor sets its thread and browser handles before anything can
  fail (`RETAIL_BUGS.md`; src/Game/Libs/GameSpy/WinSync.h,
  GameSpyBrowser.cpp).
* **GAME-1: GetCommandName.** Returns the name it finds instead of always
  its error string (`RETAIL_BUGS.md`;
  src/Game/ClientShellDLL/ProfileMgr.cpp).
* **GAME-2: the chassis's update.** The update a chassis sends the
  clients names SFX_CHASSIS_ID and the chassis, as every special-fx
  message does (`RETAIL_BUGS.md`; src/Game/ObjectDLL/Chassis.cpp).
* **GAME-3: the join screen's lock.** The server browsers' thread queues
  each server it finds under a lock of its own, and the screen files the
  queue on its render, so neither thread waits for a lock the other holds
  (`RETAIL_BUGS.md`; src/Game/ClientShellDLL/ScreenJoin.cpp, .h).
* **GAME-4: a body's spears.** RemovePowerupObjects clears each spear's
  own touch notify, not the handle past the end of the weapons' array
  (`RETAIL_BUGS.md`; src/Game/ObjectDLL/Body.cpp).
* **GAME-5: the message queue's lost message.** With no active message
  to reuse, CHUDMessageQueue::AddMessage keeps the one it allocates
  instead of writing through NULL (`RETAIL_BUGS.md`;
  src/Game/ClientShellDLL/HUDMessageQueue.cpp).
* **APPS-1: the launcher's own path.** The game's path is written into
  `sExe` with a bound (`RETAIL_BUGS.md`; src/Game/Launcher/Launcher.cpp).
* **APPS-2: the launcher's find handles.** ExistRezFile, ExistRezDir and
  CDisplayMgr::EnumerateRenderers close their `_findfirst` handles
  (`RETAIL_BUGS.md`; src/Game/Launcher/RezFind.cpp, DisplayMgr.cpp).
* **APPS-3: the dedicated server's console.** SCMD replies are written as
  text, not as a format (`RETAIL_BUGS.md`;
  src/Game/ServerApp/ScmdConsoleDriver_ServerApp.cpp).
* **APPS-4: the launcher needs no disc.** ContractJack.exe looked for the
  game disc at every start and every Play: the `CdRom Drive` registry
  value (nothing writes it), then `OpenFile` of `\ContractJack.exe` on
  every optical drive, an empty one too. It never refused to play without
  it (CD_ExistGame always succeeds, so the "insert Disc 1" prompt cannot
  come; the disc check was SecuROM's, in the retail Lithtech.exe, which
  this engine does not have); the drive was only a fallback place for
  GAME.REZ. In the modern build no drive is probed and the archives come
  from the game directory only; the message for a missing Game.rez no
  longer mentions a disc (or "Contract J.A.C.K. 2", `RETAIL_BUGS.md`)
  (nolf2src's M-L4, nolfsrc's M-P1-15; src/Game/Launcher/Launcher.cpp
  CD_GetDriveLetterWithGame, TO2/RES/Lang/EN/Launcher.rc). Tested under
  Wine in a fresh prefix with drives C: and Z: only: Play, the detail
  dialog, the game to its main menu.
* **APPS-5: the launcher plays the game it is in.** Without the registry
  values the retail installer (run from the disc) leaves under
  `HKEY_LOCAL_MACHINE\SOFTWARE\Monolith Productions\Contract Jack\1.0`
  -- an unpacked or copied game, a fresh Wine prefix -- the retail
  launcher found no rez files, offered only "Install", and that started
  the disc's `SETUP.EXE` ("Unable to launch the setup program. Please make
  sure that your Contract J.A.C.K. CD is in the drive"). In the modern
  build the folder ContractJack.exe is in is the game whenever
  Lithtech.exe is beside it, whatever `InstallDir` says (InstallDir only
  where it is not); the key is made when it is missing, so the launcher's
  settings are kept, and when it cannot be (no right to write the
  machine's key) the launcher plays with its defaults. With no
  `UpdateNum` the command line gets the 1.1 update's `-rez
  Update_v1x1.rez` (if the file is there), as its installer's
  `UpdateCommandLine0`; `-rez custom` moves after the updates, so the
  update's game DLLs (emptied in this build's archives) never override
  ours. Uninstall (the installer's setup.exe) is disabled; "Install",
  left only to a launcher away from its game, says the game is not
  installed properly instead of looking for the disc (nolf2src's M-L6;
  src/Game/Launcher/Launcher.cpp InitInstance, OpenRegistryKey,
  LauncherDlg.cpp OnInitDialog, OnPlay, LaunchGame). The key is the
  user's (APPS-7). Tested as APPS-4: the launcher offered Play with no
  registry values, and its command line ended `-rez Update_v1x1.rez -rez
  custom`.
* **APPS-6: no CD key.** A server published with GameSpy (every multiplayer
  server, a LAN one too) sent each joining client a CD-key challenge, had
  GameSpy's servers check the answer, and turned the client away with
  "CD-Key verification failed" (MID_HANDSHAKE_CDKEYREJECTED) when the
  check failed -- as it does for a client with no key, whose answer is
  empty; and the multiplayer screen asked for a key at once and kept Join
  and Host off until one was typed. GameSpy's key servers are gone. In
  the modern build the server sends no challenge (the handshake message
  keeps its flag, set to "none", and MsgIDs.h keeps every value) and lets
  the client in when its handshake is done, so no client waits for a key
  check (UpdateClientCDKeyChecks finds none, and never rejects one); the
  multiplayer screen hides the key and never asks for it. The client's
  answer to a challenge (ClientMultiplayerMgr) is left as it is: a modern
  server sends none, so it does not run. A modern client is not meant to
  join a retail server (nolf2src's M-G17;
  src/Game/ObjectDLL/GameServerShell.cpp ProcessHandshake,
  src/Game/ClientShellDLL/ScreenMulti.cpp OnFocus). `MPTEST_CDKEY=none`
  runs build/mptest.sh with no key in the registry. Tested: `make mptest
  MPTEST_CDKEY=none` on the modern build, no CDKey anywhere in the Wine
  prefix: the dedicated server hosts DM_Khios and the client joins and
  plays.
* **APPS-7: settings in the user's registry.** The launcher, the game DLLs
  (CVersionMgr: the profile last used, the CD key, the cheats' and the
  missions' flags) and the dedicated server kept their values under
  `HKEY_LOCAL_MACHINE\SOFTWARE\Monolith Productions\Contract Jack\1.0`,
  opened for writing even to read (`RETAIL_BUGS.md`). In the modern build
  they use the same path under HKEY_CURRENT_USER, made on the first run.
  As in nolf2src (M-L7, M-G71), nothing is carried over from the
  machine's key: the launcher needs none of it (APPS-5), and everything
  else starts at its default (the detail dialog comes once more, on the
  first Play). The dedicated server hosts without the key -- the retail
  server did not host at all without the machine's -- and, with no update
  values in the user's key, adds `Update_v1x1.rez` itself, as the launcher
  does (nolf2src's M-G50) (src/Game/Launcher/Launcher.cpp
  OpenRegistryKey, src/Game/Shared/VersionMgr.cpp CVersionMgr::CVersionMgr,
  src/Game/ServerApp/ServerDlg.cpp DoHost, AddResources). Tested under
  Wine in a fresh prefix: the launcher (Play, the detail dialog) and the
  game wrote their values, `ProfileName` among them, under the user's key
  only, nothing under the machine's; a second launcher run read them and
  started the game without the detail dialog. In build/mptest.sh the
  modern server hosted from the user's key, which has none of the values
  the script writes under the machine's.
* **APPS-8: no movies on a disc.** CWinUtil::GetMoviesPath, when the game
  directory has no `intro.smk`, looked for a `Movies\` folder on every
  CD-ROM drive. Nothing calls it; the modern build drops the drive scan
  (src/Game/Shared/WinUtil.cpp).
* **EN-1: the world shaders' section order.** The sections compare by
  world only among equal base sections, so the sort gets a strict weak
  ordering (`RETAIL_BUGS.md`; src/Engine/runtime/render_a/src/sys/d3d/
  d3d_rendershader_base.h).
* **EN-2: the window keeps its renderer.** In a window the game no longer
  shuts its renderer down and brings it back when it loses the focus (a
  reload of every texture and a black window meanwhile; only a full-screen
  device needs it); it pauses, unless the console variable
  RunInBackground is set, when it plays on behind other windows
  (src/Engine/runtime/kernel/src/sys/win/client.cpp; nolf2src's M-G41).
* **EN-3: ShowVersionInfo.** ILTClient's GetVersionInfoExt and
  GetPerformanceInfo are set, and the console command tests the first
  before calling it (`RETAIL_BUGS.md`; src/Engine/runtime/client/src/
  clientde_impl.cpp, consolecommands.cpp).
* **EN-4: the music thread's handle.** CLTDirectMusicMgr::Term closes the
  notification thread's handle (`RETAIL_BUGS.md`; src/Engine/runtime/
  kernel/src/sys/win/ltdirectmusic_impl.cpp).
* **EN-5: a failed transition segment.** CLTDirectMusicMgr::ChangeIntensity
  tests the transition's segment state before using it, as NOLF2's does
  (`RETAIL_BUGS.md`; src/Engine/runtime/kernel/src/sys/win/
  ltdirectmusic_impl.cpp).
* **EN-6: batched quads keep their colours.** DrawPrim for screen-space
  LT_POLYGT4 copies each vertex's own colour into its triangle list
  (`RETAIL_BUGS.md`; src/Engine/runtime/render_b/src/sys/d3d/
  d3ddrawprim.cpp).
* **EN-7: no sound driver.** SoundSys leaves the driver null when no driver
  can be made, rather than calling Term through the null pointer
  (`RETAIL_BUGS.md`; src/Engine/runtime/sound/src/soundmgr.cpp).
* **EN-8: DrawPrim's viewport.** CD3DDrawPrim::SetCamera gives the viewport
  it sets the camera's (or the screen's) width and height
  (`RETAIL_BUGS.md`; src/Engine/runtime/render_b/src/sys/d3d/
  d3ddrawprim.cpp).
* **EN-9: the player-view model's aspect.** The really-close projection
  takes the screen's aspect rather than the PVModelAspect console variable
  (1 by default), so the weapon model is not stretched on a wide screen
  (src/Engine/runtime/render_a/src/sys/d3d/3d_ops.cpp).
* **EN-20: the ##gamepad device.** The engine read DirectInput joysticks
  and gamepads: no dead zone, no hot-plug, an Xbox pad's two triggers on
  one axis, and under Wine a pad shows up as more than one device. The
  modern build adds the controller as a device of its own, `##gamepad`,
  read through XInput (`xinput1_4`, `1_3` or `9_1_0`, loaded at run time;
  the DirectX 8.1 SDK predates XInput, so its two types are declared by
  hand): an object behind `IDirectInputDevice8A` that answers the calls the
  engine makes of a gamepad from XInput's state -- the first connected pad
  of four. Six absolute axes (`Left Stick X`/`Y`, `Right Stick X`/`Y`
  -1000..1000, y down positive; `Left`/`Right Trigger Axis` 0..1000) and
  sixteen buttons (`A`/`B`/`X`/`Y Button`, `Left`/`Right Bumper`, `Back`,
  `Start`, `Left`/`Right Stick Click`, `D-Pad Up`/`Down`/`Left`/`Right`,
  `Left`/`Right Trigger` past XInput's threshold), buffered in whatever data
  format the engine sets; acquired only while the game has the foreground.
  It is a DirectInput gamepad (`DI8DEVTYPE_GAMEPAD`), so the game's own
  gamepad support takes it as the third device of the bindings
  (`HasGamepad`, `CProfileMgr::SetDeviceData`, Configure controls'
  capture), and it exists whenever XInput loads, so a pad can be plugged
  in later. Empty slots are looked through once a second by a thread of
  the engine's own, never in the frame (asking XInput about an empty slot
  can take milliseconds on Windows: nolf2src's M-E59). `PadHoldButtons`
  lets the client keep buttons from the bindings while it reads them itself
  (a decision, CL-22): a button pressed then stays released for the engine
  until it is let go. With `CJACK_PADTEST` naming a file, the pad is that
  file's one line of state (`padread.h`), for a scripted test. No force
  feedback (Planned). As nolf2src's M-E20 and M-E59, nolfsrc's M-P7-01
  (src/cjacksrc/xinput/gamepad_device.h, padread.h;
  src/Engine/runtime/kernel/src/sys/win/input.cpp input_Init,
  DeviceEnumCallback, input_EnableDevice, input_ReadInput).
* **EN-21: no DirectInput joysticks.** DirectInput's game controllers
  (joysticks, gamepads, wheels, flight sticks: `DI8DEVTYPE_JOYSTICK` ..
  `1STPERSON`) are left out of the engine's device list: the controller is
  `##gamepad` (EN-20), and under Wine an XInput pad is one of them too,
  which would bind and read it twice. Keyboard and mouse are DirectInput
  as before; `JoystickDisable` (the launcher's "Disable joysticks") has
  nothing left to disable. As nolf2src's M-E21 (input.cpp
  DeviceEnumCallback).
* **EN-22: no DirectInput device enumeration.** `input_Init` asked
  DirectInput to enumerate every attached device (`EnumDevices`), which
  walks every HID device on the machine; a Windows 11 crash report against
  the same engine code (nolfsrc's) shows an access violation in
  `RtlAllocateHeap` inside `dinput.dll` there, at startup. With EN-21 only
  the keyboard and the mouse are kept from it, so the modern build creates
  `GUID_SysKeyboard` and `GUID_SysMouse` directly and describes each with
  its own `GetDeviceInfo`: the same name, GUID and type the enumeration
  gave them. Everything that finds a device finds it by type (`##keyboard`,
  `##mouse`, `GetDeviceName`, `GetDeviceObjects`, `GetDeviceBindings`) or by
  that name, so `autoexec.cfg`'s bindings, the profiles and Configure
  controls are unchanged. The console's `ListInputDevices` still
  enumerates, but only when typed. As nolf2src's M-E33, nolfsrc's M-P7-09
  (input.cpp input_Init).
* **CL-20: the controller's sticks.** The client reads the sticks itself,
  so they do not depend on bindings (`ApplyJoystick` binds no axis in the
  modern build, CL-23). **Look**: the right stick past a radial dead zone
  (`PadLookDeadZone`), the rest of its travel rescaled to 0..1 and raised
  to `PadLookCurve`, is a turn rate -- 180 degrees a second across, 120 up
  and down, times `PadLookSpeed` and the frame time -- added to the mouse's
  turn (so it slows with the zoom); its up and down is the stick's own
  (`PadInvertY`), apart from the mouse's look and invert settings, and in
  first person only (the third person's view is held near level).
  **Movement**: the left stick past `PadMoveDeadZone` accelerates the way
  it points and sets the movement flags by octant (what the animations and
  the server see); past three-quarters of its travel it runs, below that it
  walks at that share of the walking speed (at least a third); Speed held
  turns that over, as Shift does the keyboard's run (RunLock is not used
  with the stick); not while ducking or leaning, as the keys. On the
  snowmobile (the vehicle code Contract J.A.C.K. keeps from NOLF2) the stick
  is the throttle and brake (up, down) and the steering (left, right); the
  right stick steers too, as the mouse does. The pad is read once a frame,
  in the menus too, while the game's window is the foreground one, from a
  slot a thread of CShell.dll's own finds (EN-20's reader; the thread stops
  in `OnEngineTerm`, before the DLL is unloaded). As nolf2src's M-G47,
  nolfsrc's M-P7-03 and M-P7-04 (src/cjacksrc/xinput/clientpad.cpp, .h,
  compiled as part of GameClientShell.cpp; src/Game/ClientShellDLL/
  GameClientShell.cpp Update, OnEngineTerm; PlayerMgr.cpp
  CalculateCameraRotation; CMoveMgr.cpp UpdateNormalControlFlags,
  UpdateNormalMotion, GetMaxVelMag; VehicleMgr.cpp
  UpdateSnowmobileControlFlags).
* **CL-21: the controller's default layout.** The profiles
  (`Profiles/*.prf`, from the shipped `Default.prf`) have no gamepad
  bindings. Once per profile (`[Controls] PadLayout`), the controller's
  column gets: right trigger Fire, left trigger Zoom, A Jump, B Duck, X
  Activate, Y Reload, right and left bumper next and previous weapon (the
  chooser; Fire takes the weapon, Start puts it away), Back Mission (the
  in-game menus), left stick click Speed (run), right stick click Center
  view, d-pad up Next ammo, down Last weapon, left Manual aim, right Move
  body; Start is the menu (CL-22). Contract J.A.C.K. has no holster or
  flashlight command (nolf2src's d-pad down and left) and adds Last weapon
  and Manual aim, which take their places. A command the player has bound
  on the controller, and a button already bound, are left alone. A new
  profile and Restore defaults get the layout too (`Default.prf` has no
  `PadLayout`). As nolf2src's M-G60 (clientpad.cpp Pad_DefaultBindings;
  ProfileMgr.cpp LoadControls).
* **CL-22: menus, cut-scenes and decisions with the controller.** Outside
  play -- the menus and screens, a message box, a popup, the in-game menus,
  a paused game, the splash screens and movies -- the d-pad or the left
  stick is the arrow keys (repeating after 0.4 s, every 0.12 s), A Enter, B
  and Start Escape; in play Start is Escape (the menu, or the chooser put
  away). A, in a cut-scene, skips it, and once dead goes on to the
  mission-failure screen (both want Space, or any key; A's binding is a
  command, not a key). While a dialogue decision is up the d-pad moves a
  highlight (red) over its choices, A takes it and B declines, and those
  buttons are kept from the bindings meanwhile (EN-20's
  `PadHoldButtons`), so choosing does not jump. The keys go through the
  client shell's own `OnKeyDown`/`OnKeyUp`, as the engine delivers a key;
  not while Configure controls waits for a control to bind. As nolf2src's
  M-G61 (clientpad.cpp Pad_Update; HUDDecision.cpp, .h PadMove,
  PadChoose; ScreenConfigure.cpp, the capture).
* **CL-23: the Controller screen.** Controls loses the retail "Use
  joystick" toggle and "Configure joystick/gamepad", whose axis set-up
  would bind the controller's sticks a second time, for "Controller": the
  joystick screen, rebuilt with Look speed (25-300%), Invert look, the look
  and move dead zones (0-50%) and Restore controller defaults (the
  controller's column of the bindings only, after a yes/no box). The
  controller's buttons are bound in Configure controls like keys, and
  named there "Controller A Button" and so on. Strings 31100-31114. As
  nolf2src's M-G62, nolfsrc's M-P7-06 (src/Game/ClientShellDLL/
  ScreenControls.cpp, ScreenJoystick.cpp, .h, ScreenConfigure.cpp (the
  device's name), ProfileMgr.cpp ApplyJoystick; src/Game/ClientRes/Shared/
  ClientResShared.h, Lang/EN/ClientResShared.rc).
* **CL-24: a gamepad's buttons bound by name.** Configure controls bound a
  gamepad's button by its instance number (`##1`), which the engine takes
  for the first object of that number -- an axis (`RETAIL_BUGS.md`). The
  modern build names a joystick's and a gamepad's controls by name, as
  retail meant to. As nolf2src's M-G63 (src/Game/ClientShellDLL/
  ScreenConfigure.cpp Bind).

Not tested: EN-20 .. EN-22 and CL-20 .. CL-24 are built (`make modern`),
but no controller was at hand and the game was not run with them: neither
a pad (nor the test pad, `CJACK_PADTEST`), nor the keyboard and mouse
found by GUID (EN-22), nor the Controller screen's layout in Contract
J.A.C.K.'s menus. nolf2src tested the same design under Wine with its
test pad.
* **EN-30: widescreen field of view (Hor+).** The game sets one pair of
  view angles for every screen (`FovX` 90, `FovY` 78: a 4:3 view), so a
  wider screen showed the same view stretched sideways. A view of the world
  drawn over the whole screen (`RenderCamera`) wider than 4:3 now keeps the
  vertical angle and widens the horizontal one to the screen's shape
  (`tan(x/2)` scaled by the width over 4:3): more is seen at the sides,
  nothing is stretched. The interface's 3D objects (`RenderObjects`: the
  menus' art) widen the same way, to stay beside the centred 2D page
  (CL-30). The camera's own angles, which the game reads back for zoom and
  its screen effects, are not changed, only the scene the renderer is
  given; at 4:3 nothing changes. The code came with nolf2src's engine
  (its M-E1) and is documented and checked here (src/Engine/runtime/
  client/src/clientmgr.cpp `CClientMgr::Render`, `cm_WidescreenFovX`).
* **EN-31: a window at any size.** The renderer wanted a display mode of
  the window's size in the adapter's list even in a window
  (`PickDefaultMode`: "Can't find an appropriate display mode!"), so a
  window could only be a size the screen could switch to. In a window any
  size is taken: the first mode in the desktop's format, at the window's
  size. From nolf2src's engine (M-E2) as EN-30 (src/Engine/runtime/
  render_a/src/sys/d3d/d3d_shell.cpp).
* **EN-32: the window mode.** Retail sized a window for a framed window's
  borders without giving it that frame (the engine creates it captioned
  only, and the game then resized it whole to the mode, `SetWindowSize`,
  so the picture lost the frame's width), and a full-screen window kept
  whatever style it had, so going from full screen to a window could
  leave it without a frame. The engine now sets the window's style with
  every mode, from `Windowed` and the new `WindowMode`: full screen, a bare
  window on top; `WindowMode` 1, a framed window whose inside is the
  mode's size, centred; `WindowMode` 2, a borderless window over the whole
  desktop at the desktop's own resolution (the registry's mode, not the
  screen's, which in full screen is the game's): `r_InitRender` makes the
  mode asked for (the Display screen's, a command line's `++ScreenWidth`)
  the desktop's before the renderer starts, and `ScreenWidth`/
  `ScreenHeight` and the game's render mode are what it got, so the
  interface lays itself out for it. Unset (0, a config from before it), a
  window as large as the desktop is borderless and a smaller one framed.
  The desktop is the primary monitor (`SM_CXSCREEN`/`SM_CYSCREEN`): under
  Wine the desktop window spans every monitor. Both window modes are
  windowed devices, so losing the focus keeps the renderer (EN-2). The
  game no longer resizes the window (nolf2src's M-G42, M-E30, nolfsrc's
  M-P1-22; src/Engine/runtime/kernel/src/sys/win/render.cpp
  `r_InitRender`, render_a/src/sys/d3d/common_init.cpp `d3d_Init`,
  src/Game/ClientShellDLL/GameClientShell.cpp `SetWindowSize`).
* **EN-33: a letter wider than its line.** `CUIFormattedPolyString_Impl::
  Parse` broke a word before a letter wider than the wrap width even when
  the letter started the word: the word then ended at index -1 (65535),
  the parse stepped back onto the same letter until the word table was
  full, and the word widths read 64K letters past the string. A letter
  that starts its word now stays on its line. nolf2src found it as a crash
  at 3840x2160 (a HUD column given its letters' new height before its new
  width); the parser is the same here (its M-E31; src/Engine/runtime/ui/
  src/cuiformattedpolystring_impl.cpp).
* **EN-34: player-view effects at wide resolutions.** The draw primitives
  drawn in the weapon's space (the really-close transform) take their
  aspect from `PVModelAspect`, the one they were made with at 4:3, and
  would stretch on a wider screen; it is widened by the screen's shape
  over 4:3, as the player-view models' own projection takes the screen's
  shape (EN-9). From nolf2src's engine (M-E15) as EN-30 (src/Engine/
  runtime/render_b/src/sys/d3d/d3ddrawprim.cpp).
* **CL-30: menus and HUD at wide resolutions.** The interface is laid out
  on a 640x480 page scaled by `width/640` and `height/480`
  (`CInterfaceResMgr`); most controls scale both ways by the width's ratio,
  so on a 16:9 screen the menus were stretched and ran off the bottom of
  the screen, and the HUD was stretched. On a screen wider than 4:3 the
  page is now scaled by the height alone and centred: the side margin is
  one offset (`CInterfaceResMgr::GetXOffset`), which every control takes in
  `CLTGUICtrl::SetScale` (and the list's, the button's image, the HUD
  message's icon), `ConvertScreenPos`/`ConvertScreenRect` add, and the
  places that place text from the ratios themselves add (help text, the
  loading and post-load screens, credits, the chat prompt, the scope, the
  dead player's dot). The interface camera widens to match (EN-30), so the
  menus' 3D art stays beside its text. The HUD keeps to the screen's
  edges: an item in the left third of the page stays at the left edge, one
  in the right third moves with the right edge, one in the middle stays
  centred (`GetHUDXOffset`, `GetHUDBaseShift`, read again in every item's
  `UpdateLayout`, which now also runs when the screen changes). Contract
  J.A.C.K.'s own items do the same: the weapon's heat bar, the bomb
  timers' icons and every `CInterfaceTimer` (the mission and team timers,
  the bomb timers' text). The in-game menu bar, which starts at the page's
  left edge, reaches on to the screen's. At 4:3 and narrower nothing
  changes. The positioning code is NOLF2's in every file that places
  something (checked file by file against nolf2src's), so its M-G2 is
  ported as it is, plus the two items NOLF2 does not have
  (src/Game/ClientShellDLL/InterfaceResMgr.cpp/.h, BaseScreen.cpp/.h,
  BaseMenu.cpp, LoadingScreen.cpp, ScreenPostload.cpp, Credits.cpp,
  InterfaceTimer.cpp, HUDMgr.cpp and the HUD items: HUDAir, HUDAmmo,
  HUDBombTarget, HUDCarrying, HUDChatInput, HUDCompass, HUDCrosshair,
  HUDDamage, HUDDistance, HUDDoomsday, HUDHealth, HUDHiding, HUDMessage,
  HUDMeter, HUDObjectives, HUDRadar, HUDRespawn, HUDWeaponHeat;
  src/Game/Libs/LTGUIMgr/ltguictrl.cpp/.h, ltguibutton.cpp,
  ltguilistctrl.cpp). `tools/playtest.sh` maps menu clicks for this layout
  in a modern game directory (`PLAYTEST_MAP=modern`).
* **CL-31: widescreen resolutions.** The Display screen listed only exact
  4:3 modes; it now lists every mode at least as wide as 4:3 (5:4 stays
  out: EN-30 only widens). With no mode listed at all (a display whose
  only modes are narrower), leaving the screen read the empty list at
  index -1; it now sets no resolution then (nolf2src's M-G1, M-E31;
  src/Game/ClientShellDLL/ScreenDisplay.cpp `GetRendererData`, `OnFocus`).
* **CL-32: window mode on the Display screen.** "Window mode", under the
  resolution: Full screen, Window, Borderless window (`Windowed`,
  `WindowMode`: EN-32). Borderless greys the resolution out (the
  desktop's is used). Leaving the screen with another mode restarts the
  renderer once at the chosen resolution (a framed window as large as the
  desktop or larger takes the largest listed resolution that fits inside
  it), then the camera, the profile's resolution and the cursor follow the
  mode the engine gave; a window's size the screen has no mode for (EN-31)
  is listed, so the screen shows it. A window after full screen is no
  longer held to the full-screen rectangle by the cursor clip.
  `display.cfg` (what the launcher reads) keeps `Windowed` and
  `WindowMode` too (nolf2src's M-G40, M-G70;
  src/Game/ClientShellDLL/ScreenDisplay.cpp/.h, GameClientShell.cpp
  `OnEvent`, ProfileMgr.cpp `SaveDisplaySettings`, ClientRes strings
  31000-31001, 31200-31202).
* **CL-33: scope and camera overlays at wide resolutions.** The masks of
  the scope and the camera are made for a 4:3 view: with EN-30 they keep
  their shape and cover the centred 4:3 area, and the world showed past
  their sides. While either is up the margins are filled black (nolf2src's
  M-G34; src/Game/ClientShellDLL/HUDCrosshair.cpp `Render`).
* **CL-34: the zoomed view's shape.** `CPlayerMgr::UpdateCameraZoom`
  scaled the vertical angle with the horizontal one (`fovY = fovX * FovY /
  FovX`), but a view's shape is the ratio of the angles' tangents: zoomed
  in, the view came out about 7% flatter than the normal one. The zoomed
  view now keeps the normal view's tangent ratio, and EN-30 widens it as
  it does the normal view (nolf2src's M-G35;
  src/Game/ClientShellDLL/PlayerMgr.cpp).
* **CL-35: cinematics in 4:3.** With EN-30 a cutscene on a wide screen
  shows more at the sides than it was framed for. The Display screen's
  "4:3 cinematics" (`Cinematics43`, off by default) covers the sides
  beyond the centred 4:3 area while the letterbox is up, fading in and out
  with it (nolf2src's M-G36; src/Game/ClientShellDLL/InterfaceMgr.cpp
  `UpdateLetterBox`, ScreenDisplay.cpp, ClientRes strings 31010-31011).
* **APPS-30: widescreen resolutions in the launcher.** The Display
  dialog lists every 32-bit mode at least as wide as 4:3, as the game's
  Display screen does (CL-31) (nolf2src's M-G1;
  src/Game/Launcher/DisplayMgr.cpp `CRenderer::Init`).
* **APPS-31: window mode in the launcher.** The Display dialog has the
  same choice under the resolutions: Full screen, Window, Borderless
  window (the resolutions greyed out for it). It starts at the mode the
  game was left in (`autoexec.cfg`, then `display.cfg`, as the engine
  reads them) and, with OK, passes `+Windowed` and `+WindowMode` with the
  resolution. `linux-run.sh` no longer forces `++windowed 1` in the modern
  build, so the menu's choice stays (nolf2src's M-L5;
  src/Game/Launcher/DisplayDlg.cpp/.h, DisplayMgr.cpp/.h,
  TO2/RES/Lang/EN/Launcher.rc, TO2/resource.h;
  build/game-scripts/linux-run.sh).
* **APPS-32: a fresh install's display.** The first run's resolution was
  the detail setting's (High 1024x768, Medium 800x600, Low 640x480),
  full screen unless `linux-run.sh` added `++windowed 1`. A fresh install
  is one where the game has never run: no `display.cfg` (the game writes
  it at every start) and no `Windowed` in `autoexec.cfg`. There the
  launcher passes its default display, with or without its Display dialog
  (which shows it, and whose Cancel leaves it), and the detail setting
  keeps only its other settings: a framed window (`Windowed 1`,
  `WindowMode 1`), 32-bit, at the largest size whose window, frame and
  title bar included, fits the desktop's work area, of the desktop's
  shape if one fits (1600x900 on a 1920x1080 desktop), else of any. A
  window can be any size (EN-31), so the common 4:3, 16:9, 16:10 and 21:9
  sizes are candidates beside the adapter's modes, and one with no mode is
  added to the dialog's list. On a Steam Deck (`SteamDeck=1`) full screen
  at the panel's 1280x800, or the largest mode that fits it; with none, a
  borderless window. A later run passes nothing, so Options > Display's or
  the dialog's choice stays (nolf2src's M-L8;
  src/Game/Launcher/DisplayMgr.cpp/.h `ModernIsFreshInstall`,
  `ModernSetDefault`, LauncherDlg.cpp `LaunchGame`).
