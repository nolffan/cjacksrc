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
J.A.C.K.'s code first): widescreen menus, HUD and field of view; mouse look
without the retail input's repeated last event; a frame limiter; windowed
modes at any size; our Bink player on FFmpeg in place of RAD's
`binkw32.dll`; Internet play without the dead master servers.

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
  LauncherDlg.cpp OnInitDialog, OnPlay, LaunchGame). The key stays under
  HKEY_LOCAL_MACHINE, where the game's CVersionMgr keeps the profile name
  and the CD key too; nolf2src's move of both to HKEY_CURRENT_USER (its
  M-L7, M-G71) needs src/Game/Shared/VersionMgr.cpp as well. Tested as
  APPS-4: the launcher offered Play with no registry values, and its
  command line ended `-rez Update_v1x1.rez -rez custom`.
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
