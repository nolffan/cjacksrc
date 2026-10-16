# The plan

Contract J.A.C.K. from source, split into packages that can each be picked
up again after an interruption. The first aim is **a game that runs** with
every binary built here; the second is **byte-identity** with retail 1.1
(the legacy build), ratcheted by `make progress`. The modern build
(`CJACK_MODERN`) comes after.

Unlike NOLF and NOLF2, no source of Contract J.A.C.K. was ever released.
What there is, and what it is good for:

| Source | What it is | For |
|---|---|---|
| NOLF2's source release (`../nolf2src/src/Game`, 1.3, 2002) | Monolith's TO2 game tree: ClientShellDLL, ObjectDLL, ClientFxDLL, ClientRes, ServerRes, Launcher, ServerApp, the libraries | the game DLLs, the launcher and the dedicated server. Contract J.A.C.K. was built from the same tree: its images name `C:\proj\to2\source\Game\ObjectDLL\Attachments.cpp` and `...\Game\Libs\GameSpy\Ping.cpp` |
| nolf2src's reconstructions (`../nolf2src/src/Engine`) | NOLF2's engine (Jupiter, back-ported from the 2006 tree to 2002, D3D8), SndDrv.dll, ltmsg.dll | the engine's 2002 baseline; the D3D8 renderer; SndDrv.dll, which Contract J.A.C.K.'s differs from mostly by compiler |
| The LithTech Jupiter tree (`../fear/Dev/Source/runtime`, `libs`, `sdk`) | Jupiter as Touchdown distributed it in 2006 ("Build 69"), D3D9, built with VS.NET 2003 (`runtime/winbuild/lithtech/Lithtech.map`) | the engine: Contract J.A.C.K.'s (October 2003) lies between NOLF2's and this one, and shares this one's compiler |
| Build 69's game code (archive.org `Lithtech-Jupiter-System-Enterprise-Edition-Build-69`, GitHub `jsj2008/lithtech`: `NOLF2/{ClientShellDLL,ObjectDLL,Shared,ClientRes,ServerRes}/{TO2,TRON}`) | the TO2 and Tron 2.0 game code as merged into the 2006 tree | what the shared game code became after NOLF2: Tron 2.0 (August 2003) is Contract J.A.C.K.'s nearest sibling. Not a release of either game's shipped code (its interfaces are later). Our `../fear` copy lacks these directories |
| The demos (`rez/demos/contractjack_demo_{sp,mp}.exe`, 2003-10-21) | the single-player and multiplayer demos: unprotected engines (`Lithtech.exe` of 2003-10-10 and -11), `server.dll`, `SndDrv.dll`, `GameSpy.dll`, the demo game DLLs inside their `Game.rez` | the code in retail `Lithtech.exe`'s SecuROM pages, and the real import behind each redirected call (nolf2src's `tools/demoimports.py` did this with NOLF2's MP demo) |
| `tron-devbuild` (archive.org) | a Tron 2.0 developer build of 2003-08-05 with its console | strings and asserts from the same months; no symbols |
| GameSpy SDK (GitHub `nitrocaster/GameSpy`, 2007-12) | the oldest full copy of the SDK's C source found; its per-module `changelog.txt` goes back through 2003 | `GameSpy.dll`'s SDK part (serverbrowsing 2.00.2x-3x, July-November 2003): rolled back by the changelogs |

There is no Contract J.A.C.K. SDK, tool release, PDB or map anywhere that
could be found (2026-10).

## The retail game

`rez/games/contract-jack-eu-1.1` (`build/retail.sh`): the European discs
with the English 1.1 update (`rez`'s `contract-jack-eu-1.1`). The European
and USA releases' binaries are identical; only the web links differ. 1.1
replaced the engine, the launcher, the dedicated server, `GameSpy.dll` and
three game DLLs (`Update_v1x1.rez`: `CSHELL.dll`, `OBJECT.lto`,
`CRES.dll`); `SndDrv.dll`, `ltmsg.dll`, `CLIENTFX.fxd` and `SRES.dll` stay
1.0's (`GameDLL.rez`).

| Binary | What | Size | Functions (enumerated) |
|---|---|---|---|
| `Lithtech.exe` | the engine; SecuROM (`.rjes`, `.obfzv`; five encrypted pages, every import call redirected); D3D8, DInput8, D3DX8 linked in | 3.1 MB | 7,402 in its own code |
| `server.dll` | the engine's server | 532 KB | 2,601 |
| `SndDrv.dll` | the sound driver (DirectSound 8, EAX) | 57 KB | 231 |
| `GameSpy.dll` | Monolith's `Game/Libs/GameSpy` around GameSpy's SDK; imported by the engine and the game DLLs | 119 KB | 578 |
| `ltmsg.dll` | the engine's message strings | 6 KB | resources |
| `ContractJack.exe` | the launcher (MFC 7.1) | 5.0 MB, nearly all resources | 581 |
| `ContractJackServer.exe` | the dedicated server (MFC 7.1, imports `server.dll`) | 1.1 MB | 822 |
| `CShell.dll` | the game: client | 1.5 MB | 6,423 |
| `Object.lto` | the game: server objects | 2.6 MB | 9,199 |
| `ClientFx.fxd` | the game: client effects (static CRT) | 213 KB | 445 |
| `CRes.dll`, `SRes.dll` | the game's resources | 397 KB, 34 KB | resources |

Every one is Visual C++ .NET 2003 (13.10.3077): `progress/COMPILERS.md`.

## Ground rules (every package)

- **Source.** Game code starts from NOLF2's source release; engine code from
  the Jupiter tree, trimmed back to October 2003 (D3D8, not D3D9: retail
  imports `d3d8.dll`), with nolf2src's D3D8 back-port of `render_a` where it
  serves. Keep Monolith's comments where the code is kept. Where nothing
  exists, write it from the retail disassembly. Mark reconstructed retail
  behaviour that differs from the base `RETAIL:` and cite the retail
  address (`0x...`) in a comment.
- **Compiler.** `msvc-7.10-3077` for everything (cl 13.10.3077, link 7.10).
  Prove the flags per binary (`/O1` vs `/O2`, `/GX`/`/EHsc`, `/GS`: retail
  calls `__security_init_cookie`, so at least some binaries are `/GS`).
- **Project files.** Our workspace is VC6-style `.dsw`/`.dsp` (the format
  `tools/dsp2mk.py` reads), with Visual C++ .NET 2003's flags; Monolith's
  own were VS.NET 2003 solutions, which are lost.
- **Legacy vs modern.** `CJACK_MODERN` gates every fix and feature; the
  legacy build must keep compiling to the retail bytes.
- **Commits.** Small commits, each leaving the tree building.

## Packages

| # | Package | Depends on | Status |
|---|---|---|---|
| 1 | `scaffold`: submodules (`rez`, `toolchains`, `tools`), the 1.1 install in rez, VC .NET 2003 in toolchains, the build image, `make retail`/`reference`/`progress`, `targets.py` | -- | done (below) |
| 2 | `build`: `build/env.sh`, `build/build.mk` and `build/compile.sh` for VC .NET 2003 under Wine, the workspace `src/Game/TO2.dsw`, `build/all.sh` assembling a game directory, `build/variant.sh` (debug, modern) | 1 | partly: everything but the game workspace and the variants (below) |
| 3 | `small`: `ltmsg.dll`, `CRes.dll`, `SRes.dll` (resource scripts from the retail resources; `tools/rsrcdiff.py`), `SndDrv.dll` (nolf2src's reconstruction, compiled with VC .NET 2003) | 2 | `ltmsg.dll` done (retail's but for its time stamp); `SndDrv.dll` every scored function identical (225, the other 30 too small to score), the file retail's but for its template tail's order (below); CRes/SRes to do |
| 4 | `game`: `CShell.dll`, `Object.lto`, `ClientFx.fxd` from NOLF2's source release toward Contract J.A.C.K. -- first the shared code (with Build 69's TO2/TRON as the later reference), then Contract J.A.C.K.'s own (its weapons, its missions' objects, its HUD and menus), placed by the scoreboard | 2 | |
| 5 | `gamespy`: `GameSpy.dll`: Monolith's wrapper from the image, GameSpy's SDK rolled back to 2003 | 2 | builds, retail's exports and behaviour (`build/gamespytest.sh`); 163/181 of Monolith's functions identical (below) |
| 6 | `engine`: `Lithtech.exe` and `server.dll` from the Jupiter tree, trimmed to 2003, D3D8; the demos' engine for the SecuROM pages and the imports (a `demoimports.py` for these demos) | 2 | in progress: both build; `Lithtech.exe` 83.3%, `server.dll` 93.1% identical; converging (below) |
| 7 | `apps`: the launcher `ContractJack.exe` and the dedicated server `ContractJackServer.exe` from NOLF2's Launcher and ServerApp | 2 | both build with retail's resources; `ContractJack.exe` 284/344 identical (98.9%), `ContractJackServer.exe` 550/644 (98.0%): what is left is ButeMgr's register allocation and EH funclets (below) |
| 8 | `boot`: the built binaries in the game under Wine, to the menu, a level, saving and loading (`make test`, tools/playtest.sh) | 3-7 | single player boots to the menu, plays M01S01, quicksaves and quickloads; the dedicated server hosts a deathmatch world; our client and retail's join it as far as the loading screen (below) |
| 9 | `match`: byte-matching, module by module, ratcheted by `make progress` | 3-7 | |
| 10 | `modern`: the modern build (`CJACK_MODERN`): fixes (`RETAIL_BUGS.md`), widescreen, our Bink player on FFmpeg, as nolf2src's | 8 | |
| 11 | `release`: `-private`/`-public` archives, install scripts from the player's own discs and 1.1 update (`tools/mkinstall.py`), and the GitHub Actions workflow building the four public archives with `make progress` on the legacy release | 8, 10 | the machinery is done (below); the run scripts (`linux-run.sh`) wait for the game to run |

## Notes per package

### 1. scaffold

Done.

- `rez/` installs `contract-jack-eu-1.1` (`rez/tools/install.py`: the EU
  release, then the update's `Program Files` file group; `UpdateNum` 1 and
  `UpdateCommandLine0` `-rez Update_v1x1.rez`, as its setup script writes
  them), recorded in `rez/games.sha256`.
- `toolchains/msvc-7.10-3077`: the two VS.NET 2003 Professional discs from
  archive.org; the Platform SDK from `vs_setup.msi`'s merge modules. Its
  `bin/` carries the native `msvcr71.dll`/`msvcp71.dll`: under Wine with
  the builtin `msvcp71`, `c1xx` spins forever.
- `tools/` now carries nolf2src's target-driven tools (they read
  `progress/targets.py`); `retailextract.py` reads the retail layout from
  `build/retail.py`.
- `make reference` extracts the 12 retail binaries and enumerates them;
  `make progress` scores 28,282 functions, all unwritten.
- `targets.py`'s `app` boundaries are provisional (the first runtime
  member `libscan.py` finds); each is to be pinned from our own `.map` once
  the binary links, as nolf2src did.
- Unknowns: the Rich header's 0x27/8491 objects in the launcher and the
  dedicated server, and the 0x69/2067 ones everywhere.

### 2. build

- `build/build.mk` takes its workspaces from a list (`WORKSPACES`), each
  with the projects wanted from it, and skips one that is not in the tree
  yet; `progress/targets.py` has every binary `optional` until its
  workspace is here. `build/all.sh` assembles whatever is built and says
  what is missing.
- The tools environment (`build/env.sh`): `run_cl`, `run_link`, `run_lib`,
  `run_rc` with `msvc-7.10-3077`; `CJACKSRC_CFLAGS` and friends for the
  variants, `CJACK_MODERN`/`CJACK_DEBUG` resolved out of the release tree
  (`tools/releasesrc.py`, `RELEASESRC_VARIANTS`).
- To do: `src/Game/TO2.dsw` (package 4), `build/variant.sh`,
  `build/package.sh`, the game's run scripts.

### 3. small

- `ltmsg.dll`: nolf2src's `ltmsg.rc` (the five resources are NOLF2's,
  byte for byte) and an empty `DllMain`, `/MD /O2`, linked by VC .NET 2003:
  the retail file but for the link time stamp (PE header offset 0xF0).
- `SndDrv.dll`: nolf2src's reconstruction of NOLF2's driver, brought to
  Contract J.A.C.K.'s. Every function the scorer can check is identical
  (225; the other 30 are a few bytes each), and the file has retail's
  size, sections, imports and .data byte for byte. What decided it:
  - Flags: `/Ox /GF`, no `/GX` (snddrv.dsp). `/Ox` has no `/Gy`: each
    object's out-of-line functions stay in one .text in the order the
    compiler finished them, uncalled ones included (counter.obj's at the
    image's start). Without `/GX` the scoped locks get no unwind frames.
  - Precompiled header: stdafx.h (windows.h alone) made by stdafx.cpp and
    used by parsewavefile.cpp and s_dx8.cpp (which includes it first);
    counter.cpp none. Through one that takes in s_dx8.h and the STL, the
    compiler emits the header's inline functions after the file's code and
    defers a dozen functions; through none, the header constants' .bss
    order is the compiler's own rather than definition order.
  - Visual C++ .NET 2003 compiles a function only once every function it
    calls that is defined later in the file is compiled: what defers a
    function is a call forward, not inlining. The template instantiations
    come at the end, by call depth.
  - Source: three functions differ from NOLF2's (Allocate3DSampleHandle and
    AllocateSampleHandle give up when Init fails; GetACMDriverListDetails
    pushes a value-initialised element), two frees are scalar `delete`
    (RETAIL_BUGS.md), Thread_Func uses `CWinSync_Event::IsSet`, and
    winsync.h is the 2006 file. ltmem.cpp is the 2006 file without
    USELTMEM (LTMemInit and LTMemTerm empty, the initializer first, its
    critical section defined), compiled through ltmem's own stdafx.h.
  - Link: the libraries as a VS.NET 2003 project passes them, the
    project's own (ltmem, winmm, msacm32, dxguid, eaxguid) and then the
    IDE's default list: that gives each DLL's imports retail's order.
  - Scoring: `progress/extent-splits.csv` names six starts the enumerator
    misses (the uncalled counter and ltmem functions).
- Where it stops: the template tail (0x10007af0..0x1000834f) has two
  pairs of identical instantiations the linker folds (/OPT:ICF), and it
  keeps the other copy of each than retail does: `_Uninit_copy` for
  `vector<ACMDRIVERDETAILS>` (pointer and iterator versions) and
  `vector::_Ucopy` (the same two). That moves code inside the tail, and
  the vtable slots and relocations that point there. What we know of the
  rule (out/exp experiments, not kept): for functions with an exception
  handler the linker keeps the copy that comes first in the object (the
  .sxdata order), for the others the copy its reference walk reaches
  first. Retail's choices mean its object emitted the iterator
  `_Uninit_copy` before the pointer one; ours compiles the iterator one a
  round later (it waits on `const_iterator::operator!=`). Not found yet:
  the source shape that changes that order. Besides that, the image
  differs only in its time stamps (header and export directory).

### 4. game

The game DLLs build (`src/Game/TO2.dsw`); so do the launcher and the
dedicated server (package 7).

- **Tree.** nolf2src's `src/Game` as its release build compiles it, with
  `src/libs` (ButeMgr, CryptMgr, RegMgr, RegMgr32, lith, stdlith, MFCStub,
  zlib's headers). Not taken: ServerDir/WON (but `IServerDir.h`, kept as a
  header until the game's GameSpy code is written), AutoRun, STLport,
  NOLF2's launcher art, the other languages' scripts.
- **One project per DLL.** Both retail game DLLs have their code in the
  alphabetical order of all their sources, the TO2 project's and the
  shared library's mixed, then the libraries in the alphabetical order of
  their projects; Object.lto's assert names
  `C:\proj\to2\source\Game\ObjectDLL\Attachments.cpp` (NOLF2's:
  `...\ObjectDLL\ObjectShared\...`). So `src/Game/ObjectDLL` and
  `src/Game/ClientShellDLL` hold both projects' files, and `Object.dsp`
  and `ClientShellDLL.dsp` (Final only, written for this project) list
  them in that order with one precompiled header each.
- **Port.** The 2006 tree's ("Build 69") changes to the shared game code,
  merged three ways (base: NOLF2's release; about 1,000 lines in 109
  files; nolf2src's RETAIL reconstructions of 1.3 are among them), and
  FEAR's VC7 ButeMgr, CryptMgr, MFCStub, dprintf.cpp. The SDK's function
  pointers lose their default arguments (as the engine's do) and
  `CLTReference::operator=` compares its pointer directly. Not taken from
  the 2006 tree: `kInputSprintfFriendly` (CJ's LTGUIMgr edit control has
  no `%` or `\` test).
- **Flags** (evidence in `progress/COMPILERS.md`): `/MD /O2` with `/GR`
  in the TO2 projects as well as the shared ones, `/GS` in LTGUIMgr only,
  and **no `/GX`** anywhere (game DLLs and their libraries: the retail
  images have the STL's try blocks and nothing else; dropping it was
  worth 950 functions in CShell.dll and 1,760 in Object.lto).
- **Resources.** CRes.dll's strings, credits and intro are retail's
  (the string tables read from the retail image by a one-off script,
  written into NOLF2's three scripts under NOLF2's names, 45 new ids as
  `IDS_CJ_<id>`; the shared script is read before ClientRes.rc's strings,
  which is the order of the retail string tables): the file is retail's
  but for its time stamp, and so is SRes.dll (no resources at all).
- **Method that pays.** RTTI gives every retail vtable with its class
  name (complete object locators; LTGUIMgr alone has none): comparing slot
  counts with ours shows which classes gained or lost virtual functions,
  and the slots' targets (placed through the scoreboard) say where.
  Constructors give the member layout (store offsets aligned on
  instruction shapes). One header change then closes every function of
  the class: KeyFramer's object name became a `std::string` (+24 bytes),
  GameBaseLite gained `IsRegistered()`, CCharacter `SetMoveToFloor()` with
  `m_bMoveToFloor` a `bool`, CAI a hit box lying-down setter
  (`SetLyingDown`, our name) and its `bool`, CAttachments::HideAttachments
  is virtual, the save macro LOAD_STDSTRING reads through a buffer:
  Object.lto 5,375 -> 5,705 identical. Every Object.lto class's vtable now
  has retail's slot count except DoomsDayDevice (+2), DoomsDayPiece (+3)
  and VolumeEffect (derives from GameBase, +2 of its own; ScatterVolume,
  SnowVolume follow). In CShell.dll: the weapon interface's crosshair
  query comes before the shell eject position and it ends with a float
  (`GetHeat`, our name; CClientWeapon returns its member at 0x158),
  CPlayerMgr::DoActivate is not virtual, CLTGUITextCtrl has `GetFontSize`
  after SetFont.
- **Game options** (`Shared/ProfileUtils`): Contract J.A.C.K.'s are one
  polymorphic BaseGameOptions (every option of every game type, 0x94 bytes;
  GetName, Clear, Write, Read virtual) with a subclass per game type giving
  its name and defaults, and ServerGameOptions holds six (single player
  and Demolition too: `eGameTypeDemolition` is 5) and a status the server
  sets for its host (0xac). Weapons stay is new, the tag score is gone
  (from the mission settings and the scoring too). Reconstructed from
  Object.lto's ProfileUtils.obj (0x1011a900..0x1011c490). The host
  screens (ScreenHost*Options) still show NOLF2's controls.
- **Version manager and server shell**: one non-virtual CVersionMgr (no
  CTO2VersionMgr; CD key in the registry, "CJ v1.1", build 193, CJ's
  game GUID), CGameServerShell is the server shell (no
  CTO2GameServerShell), with GameSpy's server: created and published at
  the end of OnServerInitialized, fed packets in ProcessPacket, deleted in
  OnServerTerm. Not yet: the rest of OnServerInitialized's changes (it
  takes the host's options in InitGameType), the republishing
  (0x100dbcb0), the socket the engine hands GameSpy (now
  ILTServer::GetUDPSocket, vtable 0x118), CDemolitionMissionMgr, and the
  server browser (IGameSpyBrowser in ClientMultiplayerMgr and the join
  screens); the client still has NOLF2's WON code, with no directory
  created. Done on the client: GameSpy's new-version check
  (IGameSpyPatchInfo from CInterfaceMgr::Init, polled by the screen
  state, FPUpdate.exe launched with the patch's address).
- **Connecting** (with the engine's SDK, merged from master): the engine
  waits for ILTServer::FinishClientConnect before a client enters the
  world. CJ calls it once the client passed the handshake and, on a
  published server, GameSpy's CD key check: its handshake runs on the
  server shell's client records (ClientData, 0x2c bytes: state 2 waiting
  for the CD key, 3 connected; unique name; team), before there is a
  player (ProcessHandshake 0x100de370, FinishClientConnect at 0x100de85a
  and in the GameSpy callbacks 0x100dd48f, 0x100dd530, 0x100e097f). Ours
  still has NOLF2's handshake after the client enters the world and lets
  every client in from OnAddClient: a stopgap, to be replaced by CJ's
  flow. The client asks the engine for a new socket (m_hSocket -1)
  wherever it clears its StartGameRequest; CJ sets it from the NAT
  negotiation (SetupClient 0x1001f190).
- **Done this round** (one header or source change closing a class's
  functions): the in-game menus are System and Mission only, the menu bar
  right-aligned (CMenuMgr::Init 0x100844c0); the system menu shows the
  server's name and address on two lines; the controls table names its
  actions (44-byte entries, 0x1014dcc8; CBindingData keeps the name),
  drops holster/flashlight/inventory/keys/intel/status and adds manual
  aim (COMMAND_ID_MANUALAIM 95); sound and music quality are performance
  settings, not the profile's; CInterfaceTimer takes its look from Init
  and keeps the time left; MBCreate is 0x20 bytes (edit filter,
  no-escape flag at CMessageBox 0x518); CClientFXMgr has one disable
  distance; CAnimatorPlayer has a motorcycle main ("LRdM"); the unique
  player name is the server shell's (GenerateUniqueName 0x100dc160), not
  CPlayerObj's. RETAIL_BUGS.md: GetCommandName always answers its error
  string (fixed in the modern build, GAME-1).
- **Scores** (recorded): CShell.dll 5,024 identical of 6,555, Object.lto
  8,142 of 9,769 (1.29 MB), ClientFx.fxd 429 of 451. The runtime
  boundaries are pinned from our maps. Done since the round before:
  Body has no animator (CJ has no server CAnimatorBody; the client's
  CBodyFX twitches the body when the server's damage message says so);
  VolumeEffect is a GameBase with Save/Load virtuals; LTGUIMgr's edit
  control has a character filter (0x184) a message box can set;
  ChassisButeMgr (both DLLs, attributes\chassis.txt) and the chassis
  piece a character carries (CHARCREATESTRUCT 0x5c, CCharacter 0xc84);
  DebugCPrint; CHUDBombTarget and CHUDWeaponHeat (new HUDLayout keys,
  WEAPON's heat fields) and CHUDMgr's members as retail has them.
- **Chassis.** Contract J.A.C.K. turned NOLF2's Doomsday into a general
  "chassis": Chassis (Object vtable 0x101b907c: PropType + 2 virtuals,
  0x100ac470, 0x100ac700) and ChassisPiece (0x101b924c: PropType + 3,
  0x100af200, 0x100af340, 0x100aee50, and its own SetNextUpdate
  0x100ae850), with plugins, between CharacterMgr and ClientLightFX in
  Object.lto's order; DoomsDayDevice and DoomsDayPiece derive from them
  (0x100c7690..0x100c9030). Our DoomsDay* are still NOLF2's, matched by
  the scoreboard to the Chassis code. ChassisButeMgr is done; the two
  bytes after the piece in CHARCREATESTRUCT (0x60, 0x64) are not
  identified. CHUDCarrying in CJ shows the chassis pieces' carrying and
  pickup icons and a list of pieces (0x94, 16-byte entries; add
  0x10052570, remove 0x100522d0) beside its own; CPlayerMgr keeps the
  carried piece (0x17c) and CanCarryObject returns the target's FX.
- **Classes still to change** (RTTI, retail vs ours). Object.lto has
  AINodeSniper, AimMagnet (+plugin), BaseGameOptions and its
  Singleplayer/DM/TeamDM/Coop/Demolition/Doomsday subclasses,
  CAIHumanStrategyCover, CBodyState (a base), CDemolitionMissionMgr,
  CVehicleButeMgr, Chassis/ChassisPiece/ChassisButeMgr (+plugins),
  GunMount (+plugin); it has no CAnimatorBody, CHierarchicalButeMgr,
  CTO2GameServerShell (merged into CGameServerShell), CTO2VersionMgr or
  CVersionMgr. CShell.dll has CAnimator/CAnimatorBody, CMPButeMgr, the
  CRagDoll*Constraint family, CHUDBombTarget, CHUDWeaponHeat,
  CScreenHostDemolitionOptions, CSoundFX, CVehicleButeMgr,
  CVehicleWeapon, ChassisFX/ChassisPieceFX (Doomsday's pieces renamed),
  CAimMagnetFX and the game options classes; it has none of CAmmoMenu,
  CDMButeMgr, CDoomsdayPieceFX, CHierarchicalButeMgr, CMenuIntel,
  CMenuInventory, CMenuKeys, CMenuPlayer, CScreenJoinLAN,
  CScreenPlayerSkills, CSkillCtrl, CSkillPopup, CTO2VersionMgr,
  CVersionMgr. VolumeEffect is a GameBase with two virtuals of its own
  (0x1015e880, 0x1015e8a0). CHUDBombTarget keeps a timer per bomb
  (0x8c-byte entries; SetTimer 0x10051740 takes an index one past the end
  too: a candidate retail bug), fed by the display-timer message's types
  3 and up.
- **Layouts still to change**: CInterfaceMgr is right but for a few
  members in the middle; CGameClientShell (+8/+24),
  CCharacterFX (+8 from 0x90), CVehicleMgr (see below), CHUDMgr (+868 at
  0x3a04), StartGameRequest-like copies in the join screens; LTGUIMgr's
  edit control has two members more (0x180 from Create, 0x184 a
  character filter called from HandleChar, 0x101243c0); KeyFramer's
  other HSTRINGs are still HSTRINGs in retail.
- **Done this round**: Chassis and ChassisPiece (Object.lto
  0x100abf80..0x100af950) with DoomsDayDevice/DoomsDayPiece on them, all
  but a few editor-plugin functions byte-identical; ChassisFX and
  ChassisPieceFX on the client (CDoomsdayPieceFX is gone), the SFX ids up
  to 60 (60, CAimMagnetFX, still to write) and their list sizes;
  CPlayerMgr's layout (the last fire time and recoil after the jitter,
  UpdatePlayerInfo's send-less-often members, the carried chassis piece);
  CClientWeapon's 79 virtual functions and its heat (0x158..0x160,
  CreateHeatFX, RemoveHeatFX, UpdateHeat); WEAPON's and CWeaponMgr's CJ
  members in their retail places; CVehicleWeapon (0x1010c810..0x1010cf45)
  and CVehicleMgr's copy of it. The chassis's update message is a retail
  bug (RETAIL_BUGS.md, MODERN.md GAME-2).
- **Where it stopped / next**: breadth first by near count.
  - **Connection flow: done** (OnAddClient, ProcessHandshake,
    UpdateClientCDKeyChecks, BootClients, KickClientData,
    OnClientEnterWorld, RespawnPlayer; the client's HandleMsgHandshake,
    CreateGameSpyBrowsers, CMissionMgr::SetServerLevel). The preload
    screen's WON CD key validation is gone (retail keeps only the state):
    it crashed a client joining by +join. A single-player level loads
    through the new path (`make test` with M01S01). `make mptest` (the
    boot branch's harness, run here from a copy) cannot judge yet: the
    dedicated server shows "ERROR - Unable to use the selected network
    service." and loads no world, with or without these changes, so the
    client waits at "Connecting to server". Still to do: the loop at the
    end of retail OnClientEnterWorld (each other player's
    0x10110810(hClient, pPlayer), a CJ player state sent to the new
    client), CPlayerObj's handshake flag (unused now), CRC32's
    two-argument CalcFileCRC and the CRCs retail takes (weapons'
    0x10111510, the model butes' 0x1008a740).
  - CHUDCarrying: done (chassis pieces' icons, the row of keys).
  - CVehicleMgr: its layout is done; what the new members are for, and
    its use of the weapon (SetPhysicsModel 0x10107b50, the lure message
    0x101096d0, UpdateWeaponModel's vehicle branch 0x1009ee04), are not.
  - CShell's CPlayerMgr::UpdateContainers (0x1009fd80) has been near
    (one move scheduled differently) since the WEAPON/CWeaponMgr commit,
    not since the merge; reverting WeaponMgr.h did not bring it back.
  - CPlayerMgr's CJ code: StartWeaponRecoil (CamRecoil* console
    variables, 0x10099ce0), UpdatePlayerInfo's caching (0x1009f725).
  - CGameClientShell (+8/+24), CCharacterFX (+8 from 0x90), CHUDMgr.
  - The GameSpy server browser, Demolition (BaseGameOptions and its
    subclasses, CDemolitionMissionMgr, the host options screen), AimMagnet
    and GunMount.
  - From the coordinator: try the engine's LTStrCat early return in the
    game's own copies; GAME_NAME's string sits in another object's data
    in all four images.
- **Tools** (not committed, one-offs under out/): rtti.py (vtables by
  RTTI), vtcmp.py (a class's vtable, retail beside ours), ctorcmp.py
  (constructor store offsets aligned), place.py (the scoreboard's
  placements with our symbol per shipped address), layout2.py
  (displacement-only nears by class).
- **Found.** Under Wine, VC 7.1's compiler spins instead of exiting after
  a fatal error (a missing #include, C1020, ...): out-of-tree runs use a
  watchdog that kills a compile after ten minutes.

### 5. gamespy

`src/Game/Libs/GameSpy/GameSpy.dsw` builds `GameSpy.dll` (and
`GameSpy.lib`) into `src/Game/built/Final/libs/GameSpy/`, with the retail
exports exactly (names and ordinals 1-16) and the retail imports in
retail's order.

- **The header for the game**: `src/Game/Libs/GameSpy/IGameSpy.h`
  (`IGameSpyBrowser`, `IGameSpyPatchInfo`, `IGameSpyServer`, their
  `StartupInfo`s and enums, and `IsCDKeyValid`), with `GAMESPY_EXPORTS`
  defined only when building the DLL. The virtual functions are in the
  retail vtables' order, their argument lists from the retail code; names
  follow FEAR's later use of these interfaces where one exists. Retail's
  game DLLs pass game type 1 (`eGameSpyGameType_Retail`). The `game`
  package links CShell.dll and Object.lto against a stopgap import library
  made from the retail export table; at merge it switches to
  `src/Game/built/Final/libs/GameSpy/GameSpy.lib` and this header (add
  `Game/Libs/GameSpy` to its include path; the header needs only
  `<string>`; the import library's names are the retail ones).
- **Layout of the image**: Monolith's code 0x10001000..0x10006390
  (GameSpyBrowser, GameSpyCommon -- each build's gamenames, secret keys
  and product ids --, GameSpyPatchInfo, GameSpyServer, Ping), then the
  GameSpy SDK (darray.c .. sb_serverlist.c, alphabetical), then WONAPI's
  CD key check (0x10014740: RTTI names `WONAPI::CDKey`), then the runtime
  from msvcprt's operator new (0x10014e5e). `targets.py` counts only the
  first part: the SDK and WONAPI are `middleware`.
- **Score**: 163 of the 181 functions of Monolith's code identical (95% of
  its bytes); 12 are `small` (the interfaces' inline constructors and the
  like, all relocations). The six nears, and what was tried:
  - `SBCallback` (0x10001910): retail materialises the game-name match as
    a bool and stores it to the stack (`mov %al,0xf(%esp)`) before testing
    it; no local, temporary or inline helper tried reproduces the store.
  - `GetSecretKey` (0x10002fd0): only the order of `lea 0xc(%esp),%ecx`
    and `add $0xc,%esp` after strncpy differs.
  - `IsValidKeyName` (0x100031e0): the merged `return false`/`return true`
    blocks sit at their first use in retail, at their last in ours; early
    returns, nested ifs, a switch and a result variable all keep ours.
  - `ToWideString`/`FromWideString` (0x10003300, 0x10003390, never
    called): retail computes the `_alloca` size and the end pointer with
    scaled addressing where ours shares `len*2`.
  - 0x10003c50: the function table merges two empty callbacks
    (0x10003c50, 0x10003c60); both are ours, one byte each.
- **What decided the matches** (as in the commits): Monolith writes each
  check as its own `if (...) return` (the failure block then sits after
  the first test); LTMIN/LTMAX are the macro kind; the StartupInfo
  structures assign their members in the constructor body; named locals
  where retail keeps a value (the sender's address before htonl, the CD
  key's product string, the ICMP ping); client ids are unsigned; the
  property table is a `stdext::hash_map` whose traits derive from
  `hash_compare` with a comparator that has a constructor; WinSync.h is
  the Jupiter tree's (CWinSync_PulseEvent explains the double
  CreateEvent); Ping.cpp's helpers are not static (static ones get a
  register calling convention) and its assert is on line 225 (`#line`).
- **The SDK** (`GameSpySDK/`, BSD, from the 2007 snapshot of
  github.com/nitrocaster/GameSpy) is rolled back toward the July-October
  2003 versions: a 2003-style `nonport.h`/`nonport.c` (written from the
  retail functions) instead of `common/`; ghttp without SSL/XML/DIME;
  natneg protocol 2 with two servers; pt's 2003 URLs; gcdkey without
  re-authentication; ServerBrowserNew without `lanBrowse`; no query
  challenges and no split-packet flag in the browser's queries (2003 qr2
  answers neither). Still 2007 code inside most functions (sb, qr2, ghttp
  internals); the API the wrapper calls is the 2003 one (argument counts
  checked against the retail calls).
- **Behaviour** (`build/gamespytest.sh`, under Wine in the build
  container): the exports exercised offline print exactly what retail's
  do (IsCDKeyValid on 40,000 random keys included), and a server and a
  browser find each other on the loopback in all four pairings of
  retail's DLL and ours. The internet side (master server list,
  heartbeats, NAT negotiation, CD key server, patch check) cannot be
  tried: GameSpy's servers are gone (a modern-build item: OpenSpy).
- **WONAPI**: `src/Game/Libs/WONAPI/` is the part of WON's library the
  image links (CDKey, CRC16, ByteBuffer), written from the image (the
  Jupiter SDK's WONAPI is not public), built as `WONAPI.lib` with `/GX
  /GR`.
- **Flags** (`progress/COMPILERS.md`): the wrapper `/MD /Ox /GF`, no
  `/Gy`, no `/GX`, no `NDEBUG`; `wsock32.lib` through a pragma in
  nonport.c (it puts WSOCK32 last among the imports, as retail).
- **Retail bugs** (`RETAIL_BUGS.md`): the request threads' start events
  leak a handle each, the browser's destructor can use handles its
  constructor never set (both fixed in the modern build, MODERN.md GS-1),
  and the server's CD key challenge is a byte longer than it reports.
- **Next**: the six nears above; the SDK's roll back function by function
  against the retail block (0x10006390..0x10014740), module by module,
  re-running `build/gamespytest.sh` after each (sb and qr2 first: they
  are what other players' retail games talk to); the Rich header has 18
  C++ objects to our 12, so Monolith's project had more files than the
  five that carry code; the internet services through OpenSpy in the
  modern build.

### 6. engine

**Source: nolf2src's engine, brought forward.** `src/Engine/runtime`,
`src/Engine/libs/rezmgr` are nolf2src's (the Jupiter tree of 2006
back-ported to NOLF2's engine of 2002, Direct3D 8), imported unchanged
and then changed in their own commits. Why that base and not the 2006
tree: Contract J.A.C.K.'s engine is NOLF2's plus a year. Its strings
(`strings` of the single-player demo's engine against NOLF2's retail
one) are NOLF2's almost all through; its object order (string anchors,
interface registrations) is NOLF2's module for module; it imports
`d3d8.dll` and `dinput8.dll` and links D3DX8. What it has of the 2006
tree's that NOLF2 lacked, found so far (each to be brought in from the
2006 tree, or written from the retail code where the 2006 tree has
dropped it again):
- `lthread.cpp`/`ltthread.cpp` in the exe too (`LThread::Start`,
  `CSysThread::Run`); `CUDPDriver::OpenSocket`; ltmem's tracking
  categories and its `mem` command's full set (`ltmemtrack.cpp`,
  `ltmemstats.cpp`: "Full memory log", "mark", "dump");
- console variables the 2006 tree has (`Use0WeightsForDisable`,
  `MultiPassLightMatch`) and ones neither tree has
  (`NetMin/MaxUnguaranteedPeriod`, `MaxExtrapolateDist`, `DebugRB*`,
  `LockableBackBuffer`, `DisablePSGP`), and "unguaranteed data" in the
  client/server API (`GetObjectUnguaranteedData`,
  `SetObjectUnguaranteedData`, `CLTServer::FinishClientConnect`):
  Tron 2.0-era API, no source anywhere;
- the engine-variable table's entries are 24 bytes, not 20: a sixth
  field, 1 for `NewPlayerPhysics`, `UDPSimulate*` and 43 render
  variables listed by name alone after them (`TableFog`, `FogEnable`,
  `DrawWorld`, `Wireframe`, `LightMap`, ...): an anti-cheat list
  (`engine_vars.cpp`, 0x428a10 returns the table); the code that reads
  the flag is still to be found.
What NOLF2 had and Contract J.A.C.K. dropped: the `CUIVectorFont`
registration, `CUDPDriver::SendPing` and "UDP: Joining on port",
`ps\envbumpmap.ps`, `r_LoadSystemTexture`'s and `r_TermRender`'s
error tags, `ILTModel::GetModelOBBCopy`, `reallyclose_far`.

**Libraries shared with the game.** `src/libs/stdlith`, `lith` and `zlib`
are the game package's (NOLF2's source release), taken as the game
branch had them, so that the two merge cleanly, with two changes VC .NET
2003 needs: lith's `dprintf.cpp` enumerators get the 2006 tree's
`DPRINTF_` prefix (the Platform SDK's WinIoCtl.h defines `Unknown`), and
zlib is there because stdlith's `compressedio.cpp` includes it (without
it the compile spins under Wine instead of failing). **SDK change for
the game:** `sdk/inc/iltclient.h` loses three default arguments
(`GetSoundOffset`'s `dwSize`, `ClearScreen`'s `pClearColor`, `End3D`'s
`flags`): VC .NET 2003 refuses them on function-pointer members (C2383),
as the 2006 tree's header has it; the game code passes them explicitly
(`End3D(END3D_CANDRAWCONSOLE)`, `ClearScreen(..., LTNULL)`,
`GetSoundOffset(..., LTNULL)`).

**Build.** `src/Engine/runtime/winbuild/Engine.dsw`: `Lithtech`
(`Lithtech.exe`) and `server` (`server.dll`), with D3DRender, Lib_UI,
Lib_ILTSound, Lib_RezMgr, ltmem, Lib_StdLith, Lib_Lith. No STLport (the
retail images use VC .NET 2003's own library). Both `/O2`, no `/GX`,
`/GS`, `/GR` (`progress/COMPILERS.md` has the measurements). The exe
names its libraries on the link line in the order retail has their code
(ltmem, StdLith, RezMgr, renderer, sound loader, UI, Lith, then D3DX)
and `/nodefaultlib:libci.lib` (d3dx8.lib asks for VC6's iostreams).
`build/engine.sh` is nolf2src's one-object loop (`Lithtech-one x.cpp`,
`server-one x.cpp`, `<project> verify`).

**SecuROM.** `tools/demoimports.py` (now per game; `cjack` is picked by
the retail engine's sha256) uses the single-player demo's engine
(`rez/demos/contractjack_demo_sp.exe`, 2003-10-10, unprotected; Rich
header, `.rdata` and `.data` sizes and addresses identical to retail's):
`progress/engine/Lithtech-iat-calls.csv` names the import of each of
retail's 1,088 redirected calls (35 SecuROM slots in `.obfzv`; 1,039
paired exactly, 31 by context, 18 weakly); `demoimports.py extract && map
&& plain` gives `out/spdemo/Lithtech-readable.exe`. The encrypted pages
(0x536000..0x53b000) are D3DX's 3DNow! code and the CRT: no engine code.

**Scoreboard.** `progress/targets.py`: both engine images have a second
`app` range, the dynamic initializers and atexit destructors VC .NET
2003 puts after the CRT and the funclets (exe 0x539a80..0x54987e, dll
0x10064900..0x10069f9b); the exe's `purecall` is the encrypted thunk
0x538534. First recorded score: `Lithtech.exe` 5,102 identical (55.3%
of its bytes), `server.dll` 2,223 (66.6%).

**Converging** (each in its commit, with the retail addresses): the
libraries without /GX; LTObject's unguaranteed-data field; the 24-byte
engine variables and the command-line lock; ViewParams' m_mWorld;
CClientMgr's 2006-tree members and resource-tree vector; the world
shaders' shared draw range and per-world sections; the collision,
attachment and binding changes; the UDP driver (the 2006 one on the 2002
locks: pause handshake, OpenSocket, bandwidth periods, the unguaranteed
drop rate); the unguaranteed-data protocol (below). Helper agents read
the retail code module by module and wrote the analyses these came from;
the reports' open questions are listed below.

**SDK changes the game must follow** (`src/Engine/sdk/inc`, shared with
package 4; not additive, because CJ's own SDK had them):
- `iltclient.h`: no default arguments on GetSoundOffset, ClearScreen,
  End3D (VC .NET 2003 refuses them).
- `ltbasedefs.h`: LTStrCat copies after the string's end, when there is
  room (CJ's, server.dll 0x1000a97a).
- `iltcsbase.h`: two new virtuals, `OpenSocket(uint32*)` after
  RemovePing and `GetObjectUnguaranteedData(HLOCALOBJ, uint32*)` after
  GetObjectPos; `iltserver.h`: `FinishClientConnect(HCLIENT)` after
  GetClientAddr, `SetObjectUnguaranteedData(HOBJECT, uint32)` after
  SetNetFlags, `GetUDPSocket(uint32*)` after GetSaveFileVersion (retail
  CLTServer vtable 0x1006d430: 78 slots, NOLF2's 73). OpenSocket and
  GetUDPSocket are our names. **The server no longer moves a new client
  towards the world by itself**: the game calls FinishClientConnect
  (and is told of the client, OnAddClient, on its hello packet), as
  CJ's Object.lto does.
- `ltbasedefs.h`: StartGameRequest has `uint32 m_hSocket` before
  m_TCPAddress (the client joins on that socket unless it is -1; our
  constructor sets -1, which the game's inline copy should check).

**Round 2** (on master's 07d9301): the render world's occlusion and
flush-on-draw paths (BindAll, GetAABBVisibility, FlushShaders, the
early-out-visible subtree path), the twelve shader Flush instances and
the shaders' template members (Lock, AddSection, InitRenderBlocks,
SortSections and the inlined sort helpers: CJ compares the worlds of two
sections after the base sections' less-than, without an inequality test,
RETAIL_BUGS.md, fixed under CJACK_MODERN as MODERN.md EN-1); batch breaks
after lit render blocks; the player mover's blocker collision through
WorldPolys; the engine's ltmem (ltmemstats/ltmemtrack/ltmemheap of the
2006 tree, library /Ox /GF, SndDrv.dll unchanged at 225); StdLith and Lith
without /GX (the game branch's same change); render blocks through
g_Device.m_pRenderWorld, CJ's sky bounds, model-piece sorting by pointer
with std::sort, shadow queueing with a weighted tween slot, render style
loading in CJ's file layout (a render pass's Direct3D data without the
structure's padding), the back-buffer sync under LockableBackBuffer, ATI's
MultiPassLightMatch/Use0WeightsForDisable, WM_FONTCHANGE broadcasts.
**SDK change for the game:** `ltbasedefs.h`'s LTStrCat returns at once
when `destBytes` is 0, but only under DIRECTENGINE_COMPILE or __D3DREND
(the engine's projects); worth trying in the game's code too (round 3
replaced round 2's `__forceinline`, which retail's out-of-line copy at
0x419760 disproved).

**Round 3** (on master's 239fb1d): `tools/demoimports.py` maps SecuROM's
91 import calls rewritten as a direct call into a `.rjes` dispatcher plus
a junk byte, and `tools/progress.py` scores against every mapped site put
back as `ff 15 <slot>` (the Lithtech.exe target's `iat_calls`), so
RunClientApp and the like can match (tools commits on the tools clone's
master: demoimports, then progress). Then CJ's forms of
d3d_BlitToScreen3D (NOLF2's old whole-tile blit, no m_bUseOld in
BlitRequest), the D3D format tables (W11V11U10), RunClientApp's window
icons, ps_UpdateParticleBoundingBox, FillPacketFromInfo's new-object
rotation and prediction cap, cis_MaskedDraw (16-bit only), the music
manager (no COM set-up in its thread). Retail bugs listed and fixed under
CJACK_MODERN: ShowVersionInfo calls a NULL GetVersionInfoExt (EN-3), the
music thread's handle leaks (EN-4), a failed transition segment crashes
(EN-5); client.cpp's NOLF2SRC_MODERN blocks are CJACK_MODERN (EN-2).
**The game against the engine** (master's game DLLs built with this
engine): every interface holder the game's DLLs define is implemented by
the engine or a game DLL (ILTModelClient through `implements_also`), with
the same versions; every ILTClient/ILTServer function pointer is set but
ILTServer's GetPointAreas, Set/GetModelNodeHideStatus (none of which the
game calls) and the two ILTClient ones retail leaves NULL; the engine's
own holders without an implementation are retail's too, except
ILTMath.Default, which retail does not have (CLTClient::Math, still to be
looked at). Note: out/exp/try.sh compiling kernel's de_file.cpp alone
makes cl.exe spin (the full build is fine).

**Round 4** (on round 3's b0c77a8): **ILTClient has a slot the SDK
lacked.** Retail's CLTClient vtable (0x54b728, 94 entries) has a virtual
between AddParticle and StartQuery (0x409070: the "internet" driver's
UDP socket into `*phSocket`, LT_OK or LT_NOTFOUND), and no Math(); there
is no ILTMath.Default holder either. `iltclient.h` gains
`GetUDPSocket(uint32 *phSocket)` there (our name), and clientde_impl.cpp
drops CLTClient::Math and its ILTMath holder. **For the game package:**
every ILTClient virtual after AddParticle (IsLocalToServer,
Get/SetLightGroupColor, SetOccluderEnabled, SetTextureEffectVar,
GetSpriteControl, SetCanvasFn/Radius, GetMaxRadiusInPoly and the query
calls) sat 4 bytes short in the game's calls until now (retail CShell
calls SetLightGroupColor through +0x150); a rebuild picks the new
offsets up. The game does not ask for ILTMath and should not: CJ's
engine has no such interface. Also matched: CPixelShaderMgr's
InstallPixelShader/FreeShader/destructor (indexing the array each
time), COccludee::SplitPlane/SplitPlane2D (every outline is split, the
fear tree's goto left after the first) and COccluder::Occlude/Occlude2D
(resize(0), the 2D AABB early-out), d3d_OptimizeSurface (every tile but
the first loses a pixel), CClientMgr::Render (retail 0x40f080; no
DrawPrim camera around RenderScene), CD3DDrawPrim::DrawPrim for POLYG4
and POLYF4 (only the last draw's result checked),
CServerEvent::DecrementRefCount (dl_RemoveAt per node), and
BuildDeviceList's Intel rule (an Intel adapter whose caps deny hardware
T&L still gets it unless it is one of eight listed parts), which leaves
BuildDeviceList size-exact with four stack slots swapped. Then, from the
size-exact and near list: StartGameRequest's constructor sets m_hSocket
last (an SDK change the game's inline copies pick up), StartupLocal
transfers the net driver before the resources, StartShell's ConnectTCP
flag goes through a local, WriteUnguaranteedDataWithAttachments takes
WriteUnguaranteedInfo's argument order, UpdateSendTimeWithAttachments
indexes the object info per store, WriteObjectRemoves keeps 32
change-flag bits, dalloc returns NULL for size 0 again (CJ kept the check
the fear tree dropped), CollideWithPoly, SetStreamSources (streams, then
the unchecked indices) and DrawAggregateShader (resize(0)). The nears
list paired UpdateSendTimeWithAttachments with heap code at 0x4a03b0 and
0x1005e930; its real homes are 0x45fe20 and 0x10030640.

**Round 5** (on master's 412c3e7): **the SDK interfaces against
retail, slot by slot.** Every engine table the game reaches through the
SDK was compared with the shipped one, slot by slot (the placed function
at each slot, or the slot's code with its calls mapped through the
placements where small functions share a shape): ILTClient (94),
ILTServer (78, in Lithtech.exe and server.dll), ILTCommon, ILTModel,
ILTPhysics, ILTTransform, ILTSoundMgr (client and server),
ILTDrawPrim, ILTTexInterface, ILTFontManager, ILTWidgetManager,
ILTCursor (retail's table at 0x5548d8, after the cursor instance's three
slots), ILTBenchmarkMgr, ILTDirectMusicMgr, ILTVideoMgr, the message
classes, SpriteControl, the CUI fonts, poly strings and widgets, and
ICommandLineArgs. All agree. The interface getters (slots 1-5 and 49-56
of ILTClient) were checked through the holders' names ("ILTDrawPrim.
Default" and so on) beside each global. ILTClient's and ILTServer's
function-pointer members sit at retail's offsets too (all 129 and 106
stores of InitFunctionPointers / SetupFunctionPointers, at 0x40bcd0 and
0x1003d3d0). One signature differs: **GetParticleBlockersInAABB** takes
`(pos, dims, uint32* pIndices, uint32& nNumIndices)` in CJ (NULL to
count; CShell's snow airspace calls it twice, 0x100f01ed). The SDK has
that form now, with `GetParticleBlockersInAABBList` as an inline helper
for the old vector form, and **SnowFX.cpp** (game package) makes CJ's
two calls; dropping every mention of `std::vector<uint32>` from
iltclient.h moved a scheduling choice in Object.lto's
CAIGoalMgr::HandleCommand (its stdafx.h includes the header), which the
helper restores. Also: ltmem without /GX (LTMemHeapInit inlines the
group Init; CLilFixedHeapGroup::Term matches; SndDrv.dll unchanged),
CWorldParticleBlockerData::GetBlockersInAABB, si_SetObjectPos (moves only
on a change of position), and `tools/fndiff.py` reads `--end` as decimal
or `+size` as well as hex (tools clone commit 0f87cb5).

**Round 6** (on master's 0b58af7). **The unplaced functions** (1,312 in
Lithtech.exe, 451 in server.dll) were mostly not code to write: about
1,100 are the five per-file initializers of math_phys.h's float
constants (`PI_2`, `RADIANS_PER_DEGREE`...), which the shipped engine has
in only 60 objects against our 230 (it was built with precompiled or
otherwise shared headers we do not reproduce); most of the rest are
exception handlers and small COMDATs. What did matter: **the
enumeration splits functions** at pointers and jump targets inside them,
so ten large reconstructions (CClientMgr::Init, input_ReadInput,
pd_Update, DrawPrim GT4/FT4, InitLevel, CollideWithWorldModel_Poly,
SendAllObjectsUnguaranteed, DrawDynamicParticles, StairStep,
SetVertexShaderConstants) could not pair; `progress/Lithtech-screen-
exclude.csv` and `server-screen-exclude.csv` name each interior row with
its container ("NOT A FUNCTION: interior block of X (0x...)"), which took
Lithtech.exe's unwritten rows from 115 to 19. **The engine's projects
are /Ox /GF, no /Gy** (`progress/COMPILERS.md`): the shipped images keep
uncalled functions (CastRay, SetVertexShaderConstants, MaybeDrawConsole,
r_GenericTextPrint). Reconstructed: **the console's drawing** (2006
tree's, Direct3D 8: Draw, DrawSmall, DrawTextLine(s), BorderedRectangle,
CalcStringOffset, the command box's Draw, con_Draw/DrawSmall),
CD3DRenderStateMgr::SetVertexShaderConstants (the 2003 render styles
editor's copy), pd_Update's pull-back towards the server's position
past MaxExtrapolateDist, the shaders' fullbright draw-range reset,
AllocShader through the main world, DEBUG_PRINT for several console
messages, resize(0) where CJ empties vectors (only where retail shows it:
a blanket change lost 38 functions), and smaller ones. Retail bugs kept
and fixed under CJACK_MODERN: batched POLYGT4 quads lose their vertex
colours (EN-6), SoundSys calls Term through a null driver (EN-7),
DrawPrim's viewport of undefined size (EN-8); EN-9 keeps the screen's
aspect for the player-view model.

**Where it stands / next** (stopped here: score recorded at
`Lithtech.exe` 6,877 identical, 96.1%; `server.dll` 2,741, 96.2%;
SndDrv.dll 225) (breadth first, per module, with `tools/fndiff.py`,
`o2gap.py`, `vtabcheck.py`, `uncovered.py`; `progress.py --nears=<target>`
pairs some rows wrongly where a function's CSV extent is split, so check
each with fndiff against the retail address first; fndiff on
`--image=out/spdemo/Lithtech-readable.exe` reads SecuROM's calls put
back; a split function's interior rows go into the screen-exclude files):
- unwritten, small: Lithtech.exe 19 rows (2 KB: the C runtime's pieces,
  folded thunks, CClassMgr's empty stubs at 0x403d70, 0x404730),
  server.dll 7;
- structural nears not yet done: DrawDynamicParticles (pEffect kept in a
  register), CModelMgr::ForEach (_Inc inlined), MarkSectionBreaks
  (Lightmap_Texture, != inlined into ==), LoadLightGroup (push_front not
  inlined in the exe's copy), LoadRenderStyle (retail calls string's
  c_str out of line);
- nears left as register allocation or scheduling only (tried, no source
  shape found): MoveObject, RotateWorldModel, StairStep, Collide,
  QueueShadows, d3d_RenderWorldWithAggregate, r_LoadSystemTexture,
  CD3D_RenderWorld::Load and Draw, BuildDeviceList (four stack slots),
  CreateDevice, BuildFontTable, MoveAttachment, LoadPixelShader,
  SetObjectFlags, sm_AllocateID, SendAllObjectsUnguaranteed,
  SetTransformMode, ps_UpdateParticleBoundingBox, the DrawPolyGrid imul
  order;
- the textured lightmap sections have no constructors now (round 2);
  watch the modern build for code that relied on them.

### 7. apps

`ContractJack.exe` and `ContractJackServer.exe` build from NOLF2's
Launcher and ServerApp in `src/Game/TO2.dsw` (`Launcher`, `ServerApp`),
Final-only projects written for this project, with NOLF2's sources in
NOLF2's file order. Recorded: the launcher 284 of 344 functions identical (98.9% of its bytes), the dedicated server
550 of 644 (98.0%); the shared code it brought along (below) took CShell.dll to 4,937 and Object.lto to 8,133.

- **Flags** (`progress/COMPILERS.md`): both `/MD /Ox /GF` -- no `/Gy`: the
  retail images keep functions nothing calls (each dialog's
  `GetThisMessageMap` beside `GetMessageMap`, CVersionMgr's unused
  members) -- the launcher without `/GX` or `/GR`, the server's own
  objects with `/GR` and without `/GX`. regmgr32.lib without `/GX`;
  ButeMgrMfcDll and CryptMgrMfcDll are NOLF2's "Final MFC DLL"
  configurations as projects of their own (`/O2 /GX`, MFC in a DLL). The
  server links server.dll's import library. Rich header's 9 objects of
  0x27/8491: AliasObj 7.0, from `mfc71.lib`.
- **Resources**: retail's, 101 of the launcher's and 18 of the server's,
  byte for byte (`tools/rsrcdiff.py`). The scripts restate Contract
  J.A.C.K.'s dialogs, strings and version resources (VS.NET 2003's
  `VALUE` form, no explicit `\0`); the art (76 bitmaps, 8 waves, icons,
  cursor, splash) is written out of the retail images at build time by
  `build/appres.py` into `src/Game/built/retail/` (`build/compile.sh`
  runs it; the release tree gets a `rez` link for it). NOLF2's server art
  is gone from the tree. Open question for the `release` package: the
  public archive carries the built programs, so their art too.
- **Scoreboard** (`progress/targets.py`, from our maps): each image's own
  code up to the import thunks of the MFC functions called without
  dllimport (launcher 0x40b8dc, server 0x41441c), and as second ranges the
  dynamic initializers and atexit destructors of their own objects before
  the ATL module's. `progress/extent-splits.csv` names the starts the
  enumeration misses (uncalled functions after int3 padding: 41 and 49).
- **Launcher** (from the retail code): hardware sound off by default;
  "Enable Screenshots" (`+lockablebackbuffer`) in place of the animated
  load screen, the options laid out anew; registry reads with a default;
  the disc check files as literals; `+JOIN` and `+MOD <mod>` on the
  command line in place of NOLF2's `-GO*`; the company and Sierra web
  buttons only; an OnCtlColor on the main dialog; the restore-defaults
  gamma 1.04; dialog positions. Its stdafx.h includes the SDK headers the
  server's does, and it compiles the shared `VersionMgr.cpp` (CVersionMgr,
  GAMEGUID).
- **Dedicated server**: the players list without NOLF2's tags column; a
  failed LoadBinaries reports the game server's status; "Retail" as a
  literal; SCMD's game options listing with Contract J.A.C.K.'s types and
  options.
- **Shared code the game DLLs compile too** (their gains are in their
  rows): VersionMgr.cpp (NOLF2's GetNetVersion/GetNetGameName/
  GetBuildGuid for the client only, the empty two-argument member at
  Object.lto 0x1015ccb0, the GUID-named exported int all four images
  export), ProfileUtils.cpp (FixPlayerName, GetMaxPlayersForBandwidth,
  the game type ids' case order), ScmdConsole.cpp, and ButeMgr: its key
  hash is `5 * hash + tolower(c)`, and its string holder is a
  `stdext::hash_set<std::string>` (case sensitive, the standard library's
  hash and less) whose strings the items point to.
- **Retail bugs** (`RETAIL_BUGS.md`, fixed under `CJACK_MODERN`, MODERN.md
  APPS-1..3): the launcher's path overrun, its find handles, the server
  console's formats.
- **Where it stops**: the launcher's code is all placed (the 60 `small`
  rows are relocation-only); its 2 unplaced are CustomizeDlg's EH
  funclets, which retail keeps in `.text$x` beyond the scored range. The
  server's nears are two register allocations in ButeMgr (a hash_map's
  `lower_bound`, CButeMgr's constructor: which of the three CString
  members stay in registers); its unplaced are ButeMgr's EH funclets.
  Whole files: the imports are retail's in order (the server links
  `server.lib` before `winmm.lib`); RegMgr and regmgr32 are `/Ox /GF`
  (every image that links CRegMgr imports `RegDeleteKeyA`, used only by
  its uncalled members), which gives the launcher's `.text` retail's
  exact size and `.data` retail's bytes; what still moves code is which
  copy of a folded COMDAT the linker keeps (the launcher's first object
  ends Serialize/AssertValid/OnMouseMove in retail, AssertValid/
  OnMouseMove/Serialize in ours), the `.rdata` order, and the server's
  `.text`, 0xc0 bytes short. Data-only literals checked against the
  retail strings: the server's application name `ContractJackServer`,
  its registry key `Contract Jack`, its log `server.log`, the launcher's
  `CJ Modification %i`.
- **For the game package**: RegMgr is now `/Ox /GF` (CShell.dll and
  Object.lto import RegDeleteKeyA/RegDeleteValueA as retail does; no
  score change); VersionMgr.cpp's NOLF2 network strings are client-only;
  ButeMgr's string holder change and hash (above) gained 13 and 12
  functions in CShell.dll and Object.lto.

### 8. boot

- `make test` (build/playtest.sh: tools/playtest.sh with Contract
  J.A.C.K.'s names; TEST_GAME, TEST_WORLD, TEST_SCRIPT, TEST_NAME) and
  `make mptest` (build/mptest.sh: a dedicated server and a client joining
  it, under one Xvfb; MPTEST_WINEDEBUG, MPTEST_BT=1 for winedbg backtraces).
  build/playtest-play.txt is the single-player script: to the main menu,
  the first level, a little play, F6 quicksave, F9 quickload (with the
  "This will end your current game" confirmation), shots.
- Results on master 861c30e (engine round 5) plus this branch (and on
  6b59e7b, engine round 4, before it): the game
  runs to the main menu, loads M01S01 (its briefing), plays, quicksaves
  and quickloads to the saved position. The control is our Lithtech.exe
  and server.dll with retail's game DLLs (from GameDLL.rez, no `custom`):
  it does the same script identically since round 4 (before it, the
  missing ILTClient slot crashed it in its first frames). The retail
  Lithtech.exe refuses to run (SecuROM: "No CD-ROM drive found").
- Fixed here: `CNetMgr::GetLocalIpAddress` returned the driver's bool as
  an LTRESULT, so `GetTcpIpAddress` said LT_ERROR on success and the
  dedicated server stopped with "Unable to use the selected network
  service" (status 2 from CGameServerShell::OnServerInitialized). Retail
  converts it; server.dll 0x10029de0 and Lithtech.exe 0x451e90 now match.
- Multiplayer under Wine, as far as it goes: Contract J.A.C.K. ships no
  co-operative worlds (its CoopMissions.txt still names NOLF2's
  `CP_c01s01`...; a co-op server waits in its level change for ever, in
  retail too), so mptest hosts deathmatch. The server reads the missions
  file the client writes when it starts (retail: `MPMissions.txt`); with
  it, our server loads its world ("DoRunWorld completed"), as retail's
  does. A client from the command line (`+join`) then:
  - retail game DLLs on our engine, against the fully retail server:
    "CD-Key verification failed" (no CD key in the registry and GameSpy's
    servers are gone; the same with any server -- not ours to fix);
  - ours: crashes in `CScreenPreload::UpdateCDKeyValidation` (CShell,
    reading NULL): NOLF2's WON CD-key validation runs for a `+join`
    (g_bLAN false) and `ClientMultiplayerMgr::CreateServerDir` returns
    NULL in CJ. Retail has no such step: its `UpdateInterfaceSFX`
    (CShell 0x100da750) tests a member at 0x7ac instead and never calls a
    validation function. **For the game package.**
  - ours with that step skipped (an experiment, not committed): connects,
    gets its ID, enters the world (`Entering world
    WORLDS\RETAILMULTIPLAYER\DM_VOLKOVSLAIR.DAT`, `MID_PLAYER_CONNECTED`)
    and then stays on the loading screen: its switching-worlds state is
    WaitForClient and its world is loaded, but it has no client object and
    its player state stays PS_UNKNOWN, so `CPlayerMgr::IsPlayerInWorld`
    never holds; both processes run normally (winedbg). The same on
    master 861c30e (engine round 5).
  - retail's client DLLs (from Update_v1x1.rez/GameDLL.rez) on our engine
    against our server and Object.lto: first "Unable to join because the
    game server is a different version" -- NOLF2's GAME_HANDSHAKE_VER 1.3;
    retail's is 0x100 in both DLLs' .rdata, now fixed here
    (VersionMgr.cpp). With that, retail's client shows the level's loading
    screen (Volkov's Lair) and waits there like ours: so what is missing
    is on the server side -- our Object.lto's handshake
    (`CGameServerShell::RespawnPlayer` sends HELLO; `ProcessHandshake`
    checks the LETMEIN CRCs and kicks on a mismatch, which the client
    never hears of) or server.dll's connect sequence (the
    FinishClientConnect stopgap, KickClient). **Next for game/engine.**
- On master 0b58af7 (the game's connect flow), the mixed runs (mptest on
  a game directory of mixed parts; retail's DLLs from Update_v1x1.rez and
  GameDLL.rez):
  - ours against ours: no crash any more; the client gets its ID, then
    "Disconnected from server" (Disconnected(4)), before entering the
    world; the server lists no player.
  - our client (Lithtech.exe, CShell) against retail's server
    (ContractJackServer.exe, server.dll, Object.lto): the same plain
    "Disconnected from server" (Disconnected(3)), no reason shown;
  - retail's client DLLs on our engine against our server: "Invalid game
    assets." -- eDisconnect_BadFiles, sent by our Object.lto's handshake
    (one of its CRC checks: weapons, CShell file, modelbutes, surface,
    damagefx -- or the order/masking of the LETMEIN fields);
  - retail's client DLLs on our engine against retail's server: "CD-Key
    verification failed" (expected: no key, GameSpy gone).
  So both sides break the join: our server rejects retail's client at
  the asset check, and our client is dropped by retail's server (as by
  ours) before the CD-key step that retail's client reaches -- our
  client's side of the handshake/CD-key exchange differs from retail's.
- The main menu against retail's (the hybrid run): retail has no fourth
  item (ours adds NOLF2's LAN item, which CJ's CRes labels "TEAM SAY:");
  retail's version text is "v1.1 - Retail": CShell.dll's string is still
  "CJ v1.1" (VersionMgr), so retail's ScreenMain shows it without its
  "CJ " and its mod name is "Retail" with no mod (the string "Retail" is
  next to "host"/"join" in CShell.dll's data); ours shows "CJ v1.1 - ".
- The control for multiplayer is Contract J.A.C.K.'s multiplayer demo
  (rez/demos/contractjack_demo_mp.exe: `rez/tools/sfxcab.py`, then
  `rez/tools/unshield/unshield x` on its data1.cab; Program_Files and
  System_Files as the game directory, `-rez Game.rez`, MPTEST_MISSION=0):
  its own engine, server and game DLLs host DE_Italy and the client joins
  and plays it, in the same mptest run -- the harness and Wine are not
  what stops ours. (Its game DLLs do not load on our engine: "Invalid
  shell DLL", the demo's interfaces are not retail's.)
- **For the game package**: retail writes one `MPMissions.txt` for
  deathmatch and doomsday (Object.lto and CShell.dll both name it; ours
  writes `DMMissions.txt` and `DDMissions.txt`); retail's main menu has no
  LAN item (ours shows NOLF2's IDS_MULTIPLAYER_LAN item, labelled "TEAM
  SAY:" by CJ's CRes); retail's version text reads "v1.1 - Retail" (ours
  "CJ v1.1 -"); the briefing's "Press any key to continue." is orange in
  retail, blue in ours, and its text wraps differently.
- **For the engine package**: the console's "LithTech build
  4294967295.4294967295" (the build number is unset).

### 11. release

- `build/package.sh` makes `<dir>-private.tar.gz` and `-public.tar.gz` for
  every variant (`build/variant.sh`: debug, modern, modern-debug, with
  `CJACK_DEBUG`/`CJACK_MODERN`). `tools/mkinstall.py` reads CJ's discs and
  update from `build/retail.py` (`INSTALL_*`): the public archive's
  `linux-install.sh` was checked to rebuild the private directory byte for
  byte from the European discs (`.iso`) and from the USA discs (disc 1 as
  redump's `.zip`), each with the 1.1 update.
- `.github/workflows/public-archives.yml`: nolf2src's, for CJ (the redump
  zips of both releases cached; four builds; `make progress` on the legacy
  release). Lints clean (actionlint); not yet run on GitHub (nothing is
  pushed). It needs the submodules' commits pushed before the superproject.
- To do: `linux-run.sh` (and a launcher script) in the game directory once
  the engine runs, and the Windows install script tested under PowerShell.
