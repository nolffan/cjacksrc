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
- **Multiplayer join (this round)**: our client and server join each
  other, and each joins retail's (the mixed runs: a game directory whose
  `custom/` holds our CShell and retail's Object.lto or the other way
  round; the client loads CShell from it, the server Object.lto). What it
  took: retail's CRC32 goes on over the upper-cased file title after the
  bytes, and the weapons and modelbutes CRCs sum more files
  (CWeaponMgr::GetFileCRC 0x10111510, CModelButeMgr::GetFileCRC
  0x1008a740); one MPMissions.txt for deathmatch and doomsday (with the
  DE_ worlds and SelectedWeapon); CPlayerScore without tags and the
  server app's shell update with two values per player (with three, the
  server app spun for ever reading past the message once a player
  joined: that was the "loading screen for ever"); the pickup messages'
  activated flag. A join still needs a CD key in the client's registry
  (HKLM ...\Contract Jack\1.0, CDKey, any key of the right form): with
  none, client and server alike reject it ("CD-Key verification failed."
  now, as retail says it), as an all-retail run does. **The harness
  (build/mptest.sh, boot's) should write one**; a copy with the key is
  how the runs above were made. The last steps of OnClientEnterWorld are
  in (each other player's weapon sound loop to the new client).
- **Done this round** (after the join): CPlayerObj's SECRET (mission
  stats count it, IC_SECRET_ID, the client's "You found a secret." and
  sound) and ChangeModel commands, its overlay set (saved, shown again on
  load), its vehicle bytes and weapon sound loop (saved), its retail
  members up to +0x2ac0 (MissionStats has one more counter); the pickups'
  activated flag on both sides (UpdatePlayerStats reads the message
  itself, byte-identical); CInterfaceMgr::Disconnected's handshake
  messages; the scoreboard without tags.
- **Round after the join**: CVehicleButeMgr (Shared, every function of
  both copies byte-identical); vehicles are its records: PlayerVehicle's
  VehicleType names one (model, skins, WorldEdit dims), multiplayer
  vehicles respawn after ServerButes' VehicleRespawnTime and blink
  (PVFX_* messages); CPlayerObj's SetPhysicsModel(model, vehicle id,
  update client), SetVehiclePhysicsModel (Motorcycle/Snowmobile socket,
  the vehicle's weapon), RideVehicle (not ducking) all byte-identical.
  MID_PLAYER_UPDATE in CJ's form on both sides (byte flags, rotation and
  position only when changed, CSendRate_Min; server ClientUpdate and
  HandlePlayerPositionMessage byte-identical, client UpdatePlayerInfo a
  near): ours and retail's misread each other before. UpdateWeaponModel
  fires a vehicle's own weapon (CVehicleMgr::UpdateVehicleWeapon).
  Joins rechecked (ours, retail client/our server, our client/retail
  server) and M01S01 save/load.
- **Vehicles, projectiles and new objects (round after)**: CVehicleMgr
  is CJ's: handling, sounds, player-view model and weapon from the
  vehicle's record (SetPhysicsModel(model, id, preset) 0x1010abe0, the
  record's pitches and turn rates as VarTracks, SaveVehicleButes for the
  cheat), reversing and braking against the motion (UpdateVehicleMotion,
  UpdateVehicleSounds, the banking turn), the lure's saved offset
  transform (Save/Load, PlayerLureFX::SetOffsetTransform), no light cycle
  (gone from the enum too). The physics-model message carries the vehicle
  id (CMoveMgr; CPlayerObj::WriteVehicleMessage 0x1010faa0, which also
  sends CFX_PHYSICSMODEL); CCharacterFX keeps it in CHARCREATESTRUCT's
  0x60/0x64 (the physics model and vehicle id: the two unknown bytes) and
  plays the vehicle's own MP sounds; CPlayerVehicleFX blinks, moves and
  plays the spawn FX (PVFX_*). SURFACE has CJ's per-vehicle-type sounds,
  speed and impact FX (VehicleType%d_*, 6 types) instead of NOLF2's
  snowmobile fields. Object.lto: CProjectile's fire sound and ignore-FX
  word, range falloff (EffectiveVectorRange, VectorHalfDamageDist),
  VectorImpactDebug, the vector filter skipping projectiles and weapon
  items, heat-seeking projectiles (AMMO HeatSeeking*, UpdateHeatSeeking
  0x10120620, CCharacter's list of characters); AimMagnet (server side;
  CAimMagnetFX on the client is not written: SFX 60 is ignored there);
  GunMount; the GameSpy listing's keys (UpdateGameSpy 0x100dbcb0); AIHuman
  UpdateMovement's MoveToFloor and ladder case; CAIGoalSniper's SEEKPLAYER
  and NORADIUS (functional, layout not retail's). ClientShellDLL edits
  (all vehicle work): VehicleMgr, CMoveMgr, PlayerMgr (SetPhysicsModel
  calls), PlayerLureFX.h, PlayerVehicleFX, CharacterFX. Joins (ours, mixed
  both ways) and M01S01 save/load pass. Recorded: CShell.dll 5,442,
  Object.lto 8,440 identical (100 unwritten, 678 unplaced).
- **Demolition, checkpoints (next round)**: CDemolitionMissionMgr (Object
  0x100c09a0..0x100c0d50: two teams from the host's Demolition options,
  an even number of rounds, no score or time limit, MPMissions.txt); the
  game type everywhere it was missing (NetDefs' string, GameBase and
  GameBaseLite's Demolition property, the Is*GameType tables as retail's
  0x100b0ac0..0x100b0b80, the multiplayer weapon file on both sides, team
  start points, the scmd options); WorldProperties' AttackingTeam,
  AddTeamScore and CheckpointSave messages (0x1016d9c9..); CTeam's
  attacking flag (sent with the full team update) and
  CTeamMgr::SetAttackingTeam. Checkpoints on the server:
  CServerSaveLoadMgr::CheckpointSave (0x10140e60, eSaveDataCheckpoint 6,
  Save\...\Checkpoint) and the load case (0x101401b0); the client
  prints "Saving checkpoint..." (CShell 0x100212b4). Not seen in a run
  yet: only a world's CHECKPOINTSAVE message saves one, and the M01S01
  script reaches none. The Doomsday client crash does not happen on this
  master (DD_AntarcticAggression joins and plays, twice; it was there
  before the vehicle/CHARCREATESTRUCT work was merged). mptest dm three
  ways, dd, de (ours, and retail client/our server) and M01S01 pass. The
  server prints "Could not find requested variable
  'Demolition_GameState'" (a world's command variable; not checked
  against retail's).
- **Unwritten Object.lto functions (this round)**: written from retail:
  Explosion's console variables (ExKillZone, ExRadiusMulti, ExSPKillZone,
  ExSPRadiusMulti, ExIgnoreWalls, ExShowRadius; a single player's own
  blasts are bigger), its kill zone and IsBlocked (only players are
  shielded by walls); KeyFramer's LoadKeys/CreateObjectList (the object
  list is rebuilt when turned on without one); EventCounterDebug; the
  scatter and snow volumes' save/load and SFX resend, SnowVolume ON/OFF
  with StartOn (client: SNOWFX_MSG_ON; ours stops the snow at once where
  retail lets the falling snow finish); AINodeSniper (a UseObject node
  with Fov and ThreatRadius; the sniper searches kNode_Sniper nodes,
  owned by its guard node first, through
  CAINodeMgr::FindNearestNodeFromThreat's new owner argument); the
  sniper's StateComplete case; CAIGoalAttackFromCover SEEKPLAYER;
  CAIGoalAttackMelee only with a straight path; CAIGoalCheckBody ignores
  bodies for 30 s after seeing an enemy; the charge state's start
  position; Door reopens for an AI within 64 while closing;
  CDestructibleModel's world model surface from the override only;
  MID_PICKEDUP carries three flags (picked up, show, leave the item;
  PickupItem::PickedUp takes them); AmmoBox saves its model and skin;
  DisplayTimer's Type (None, Team0, Team1, BombTarget1-3: the client
  already reads it as the timer id; it was sending the team id); the
  cheat list loses CHEAT_BUILDGUID and gains mpsavevehicle (the server
  and CShell enumerations now agree with retail's: gimmegun is 16); the
  heat-seeking sort prefers characters; CWeaponMgr::ReadWeapon copies its
  string; DoVectorFilterFn caches its classes; CPlayerObj's team in
  ClientData; the respawn body goes at once; VolumeBrush show/hide
  resends the players' containers; SetPitch also sets the object's
  unguaranteed data (our client still reads CFX_PITCH, which is sent too:
  **cshell**, read GetObjectUnguaranteedData like retail's CCharacterFX);
  StartSwitchingWorlds pauses only once; CLiteObjectMgr (de)activation
  ignores unregistered objects. progress/Object-screen-exclude.csv
  lists the enumerator's interior splits (jump-table cases, blocks after
  an early return, stray targets). Object.lto: 83 -> 26 unwritten.
  Also: InitGameType takes the host's options (OnServerInitialized
  reads them), CPlayerAttachments::HideAttachments, CLiteObjectMgr::
  HasObject, the sniper goal's UpdateGoal. Still
  unwritten: the heat-seeking sort's instantiations (0x10121550..
  0x10122330; the predicate did not pair them), CSoundFilterMgr's
  constructor (0x1014a880: retail's filter list is a map and a vector,
  not a CTList), the hash compare of ObjectTemplateMgr's hash_map
  (0x10101790, inlined in ours), MFCStub CString's MakeUpper/MakeLower/
  MakeReverse (0x10173970, nothing calls them), CWinUtil::GetMoviesPath
  (0x10168110, nothing calls it in Object.lto), the vector constructor
  iterators (0x100071a0, 0x100120a0), CAnimationContext::IsPropSet
  (0x10082f10), CAIGoalInvestigate::HandleStateInvestigate's CJ changes
  (0x1002a9d0), CPlayerObj::TransferWeapons (0x10109890),
  CDestructible::DoDamage (0x100c1a00) and a few small ones the pairing
  moved (0x100dbbb0, 0x100f2390, 0x101609e0, 0x100356e0, 0x100352e0,
  0x1008f900, 0x1009eea0, 0x10110900).
- **Kept functions, and the next 13**: retail keeps functions nothing
  calls from some objects (no `/Gy`): MFCStub (CString::MakeUpper/Lower/
  Reverse, CRect::TopLeft/BottomRight in both DLLs), and Object.lto's
  WinUtil, TeamMgr, GunMount, WeaponMgr (the FX forwarders) and
  AIVolumeMgr (the GetVolume thunks): those build with `/Ox /GF`. All of
  Object.lto at `/Ox /GF` adds 78 unplaced functions, so retail's other
  objects did have `/Gy`. Merged rows are split in extent-splits.csv; the
  compiler's vector iterators are excluded (progress.py never pairs ours).
  GameSpy now gets the server's own UDP socket (ILTServer::GetUDPSocket,
  0x100dfa33): it answered on port+1 before. Object.lto: 13 unwritten,
  each a function ours has that the pairing does not place:
  0x10120b20 (the heat-seeking update since the sort predicate changed),
  UpdateSwitchingWorlds 0x100decc0, TransferWeapons 0x10109890,
  CDestructible::DoDamage 0x100c1a00, CSoundButeMgr's constructor
  0x1014a880 (CHierarchicalButeMgr and its index hash_map inlined),
  SaveOverlays 0x10110900, SetPitch 0x1009eea0 (also sends CFX_PITCH),
  HandleStateInvestigate 0x1002a9d0, ObjectTemplateMgr's hash compare
  0x10101790, CAIGoalSniper's OnLinkBroken 0x100352e0, DamageStruct::Clear
  0x10012170, 0x10082ed0, 0x101526d0. CShell's four VehicleMgr rows
  (0x101055f0 the lure's key handler, 0x10105630, 0x10107850, ...) are
  game's now and not yet done.
- **Where it stopped** (the coordinator's pause, after that commit).
  Next: 0x10121800/0x10121550/0x10122330 are std::sort's for the
  heat-seeking targets (ours instantiates its own); CJ's AI node
  interface (AINode::GetStatus takes the AI first;
  AIVolumePlayerInfo::FindViewNode takes a target, 0x10079e30) and the
  sniper's FindNearestAttractorNode (0x10035550) on it; CAIHuman has 8
  bytes more before m_bForceGround; the rest of the unwritten list,
  Demolition, the server browser, GAME_NAME, the unplaced.
- **Where it stopped / next**: breadth first by near count.
  - **Connection flow: done** (see above). The handshake flag of
    CPlayerObj is unused. CPlayerObj::SaveOverlays (0x10110900) is near
    (retail inlines the set iteration), HandleOverlayMessage near.
  - mixC (our client, retail server) showed no weapon before the pickup
    messages carried their flag; with it, it does.
  - CHUDCarrying: done (chassis pieces' icons, the row of keys).
  - CVehicleMgr: its layout is done; what the new members are for, and
    its use of the weapon (SetPhysicsModel 0x10107b50, the lure message
    0x101096d0, UpdateWeaponModel's vehicle branch 0x1009ee04), are not.
    On the server CPlayerObj's vehicle bytes (+0x2ab3 the weapon, +0x2ab4
    the VehicleButeMgr id) are in place and saved, but nothing sets them
    yet (Object 0x10109b63, 0x10109c27; 0x10109c50 resets them).
  - CShell's CPlayerMgr::UpdateContainers (0x1009fd80) has been near
    (one move scheduled differently) since the WEAPON/CWeaponMgr commit,
    not since the merge; reverting WeaponMgr.h did not bring it back.
  - CPlayerMgr's recoil: done (StartWeaponRecoil 0x10099ce0 and
    DecayWeaponRecoil 0x10098b80, both size-exact nears: one store
    scheduled differently, one jump sense); NOLF2's fire jitter and its
    console variables are gone. Still: UpdatePlayerInfo's caching
    (0x1009f725), UpdateWeaponModel's vehicle branch.
  - From the coordinator: the game's code never calls LTStrCat (nothing
    to try there).
  - CGameClientShell (+8/+24), CCharacterFX (+8 from 0x90), CHUDMgr.
  - The GameSpy server browser, Demolition (BaseGameOptions and its
    subclasses, CDemolitionMissionMgr, the host options screen), AimMagnet
    and GunMount.
  - From the coordinator: GAME_NAME's string sits in another object's
    data in all four images (not looked at).
- **Object.lto's unplaced and nears** (661 unplaced -> 0).
  - 649 were the compiler's: `_$E` initializers and `__ehhandler$`
    funclets, whose shipped twins sit above the runtime (.text$x from
    0x101752d0, .text$yc/.text$yd to the end of .text). targets.py names
    that span `compiler_output` (a tools hook): ours pair there by shape,
    or as near, and nowhere else. Placed by size before, 25 of them sat
    at game functions' addresses; those showed up as unwritten and are
    written, split or excluded now.
  - Decided: four unwritten rows stay until the class layouts land (the
    layout agent's): the AI factories at 0x10032c10, 0x1004afe0 and
    0x1004e460 are size-matched to ours (retail CAIGoalAttack and so
    CAIGoalAttackRangedDynamic are 4 bytes larger, 0xdc/0xfc;
    CAIHumanStateAttackFromCover 0x8c), and Body::OnDeactivate
    (0x10092e20) tests a bool at 0xc24 the constructor sets last.
  - Also the layout agent's: GADGETTARGET has `AmmoType` (char*, 0xb8)
    that Bombable::SetupDefusedState (0x1009cbb0) spawns the pickup from.
- **Tools** (not committed, one-offs under out/): rtti.py (vtables by
  RTTI), vtcmp.py (a class's vtable, retail beside ours), ctorcmp.py
  (constructor store offsets aligned), place.py (the scoreboard's
  placements with our symbol per shipped address), layout2.py
  (displacement-only nears by class).
- **Found.** Under Wine, VC 7.1's compiler spins instead of exiting after
  a fatal error (a missing #include, C1020, ...): out-of-tree runs use a
  watchdog that kills a compile after ten minutes.

### 4b. layouts

Class layouts and save/load against retail, in both game DLLs (branch
`layouts`).

- **Method.** Every near Save/Load (and message reader/writer) is
  compared by its call signature: the ILTMessage vtable slots called, in
  order, with the bit widths pushed (WriteBits 0x24/ReadBits 0x3c, 8, 32,
  1 ...). Object.lto's ClassDefs give every object class's size and
  property list (static PropDefs); after this round all 207 sizes and
  property lists are retail's. RTTI complete object locators give the
  secondary-base offsets: all agree. tools/layoutscan.py's uniform shifts
  pointed at the rest.
- **Save streams fixed** (the likely cause of retail crashing on our
  saves): CAI no longer saves the sight grid ranges (4 floats);
  AINodeCover saves its PlayerRadius; CAIGoalAttack its seek-player flag
  and CAIHumanStateAttackFromCover the same; CPlayerMgr saves the carried
  chassis piece's name; no NOLF2 save-version checks in any load
  (CCharacter, Body, DisplayTimer, PickupItem, CProjectile,
  StartupCommand, PropType, RadarObject, WorldProperties, CPlayerMgr).
  MID_SAVE_GAME's failure carries the level-switch flag.
- **Layouts fixed**: AINodeCover (PlayerRadius 0xc8 and its prop),
  CAIGoalAttack (0xac seek player), CAIHumanStateAttackFromCover (0x88),
  AIBM_Brain (AttackGrenadeThrowOffset 0xe4), CAIHumanStrategyFollowPath
  (door animation flag 0x38, m_vDest 0x3c), DTINFO (no bScoreTag, 0x40),
  Body (0xc24), GADGETTARGET (AmmoType 0xb8), WorldProperties (no
  AIMaxNumber prop); CShell: CTargetMgr (0x458), CHUDDistance (0x34),
  CClientWeapon (flash time before position), CScreenHostWeapons (no Add
  All), CScreenHostLevels (no loop toggle).
- **Scores**: Object.lto 8628 -> 8782 identical, CShell.dll 5757 -> 5809
  (with master's merge).
- **Left** (seen, not done): CSearchable::Enable sets only
  USRFLG_CAN_ACTIVATE in retail (no search flag); CAmmoChooser
  (WeaponChooser.obj, -432); CScreenJoin's server entry (+36);
  the plugins' layouts (CExplosionPlugin, CVolumePlugin, CAIHumanPlugin);
  CGadgetTargetMgr::Init and AIGoalSearch/DrawWeapon +4 shifts; WEAPON's
  constructor (mixed shifts); message readers whose pairing is by size
  and not trustworthy (CMissionMgr::HandleExitLevel's row,
  HandleMsgMultiplayerOptions reads a second bool after friendly fire,
  CWeapons::AddWeapon/AddAmmoBox, CPlayerObj::HandleWeaponFireMessage,
  ScmdServer's option messages). CTO2PlayerStats::Load now re-adds the
  keys to the HUD; AttackGrenadeThrowOffset is read but not yet used
  (CAIHumanStrategyGrenadeThrow::Update 0x10055f30, the game agent's).

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

**Round 7** (on master's 680b56c). The unwritten rows: Lithtech.exe 19 ->
3, server.dll 7 -> 2. Most were enumeration artefacts: CClassMgr's six
empty tick functions, the `free` thunks and two getters enumerated as
one row each (`progress/extent-splits.csv`), operand bytes inside static
initializers and 1,053 zero bytes at the end of Lithtech.exe's .text
(screen-exclude). The two fullbright shaders' PostFlush functions had
been crossed (texture: stage 1's coordinates back, 0x4fee20;
environment map: stage 3 and the env-map transform, 0x4fef90). Left:
`??_H` (0x401000), con_SetErrorLog (0x419670) and dfree (0x4234c0),
which ours have byte for byte but the scorer pairs elsewhere (identical
bodies elsewhere in our link), and one initializer (0x5429d0).
**The math_phys.h initializers** (investigated only): in retail
Lithtech.exe the five-function groups cluster in the libraries' objects
(Lib_UI and D3DRender files each have one) while only about a dozen of
the main project's ~200 objects do; the holders registered beside each
group name the objects. That is what a precompiled header gives: with
`/Yu"bdefs.h"` only the `/Yc` object and those built without the PCH
emit the header's dynamic initializers. The Lithtech project was most
likely built with a `bdefs.h` precompiled header (the game DLLs' `/Yu`
already shows the effect, 20 groups in CShell.dll). Not tried: a PCH
changes code generation elsewhere too (CAIGoalMgr::HandleCommand moved
with one header edit), the build would have to make the `.pch` before
any other object, and the gain is only the surplus `_$E` functions,
which are not scored. **SndDrv.dll's 30** are the scorer's `small` class
(functions almost all relocation, 162 bytes, not verifiable), not
mismatches. **GameSpy.dll's 6 nears** are block layout and register
choices (IsValidKeyName's early return, ToWideString's lea/shl,
SBCallback keeping its bool on the stack); tried, not found.

**Where it stands / next** (stopped here: score recorded at
`Lithtech.exe` 6,878 identical, 96.2%; `server.dll` 2,741, 96.3%;
SndDrv.dll 225) (breadth first, per module, with `tools/fndiff.py`,
`o2gap.py`, `vtabcheck.py`, `uncovered.py`; `progress.py --nears=<target>`
pairs some rows wrongly where a function's CSV extent is split, so check
each with fndiff against the retail address first; fndiff on
`--image=out/spdemo/Lithtech-readable.exe` reads SecuROM's calls put
back; a split function's interior rows go into the screen-exclude files,
missed starts into extent-splits.csv):
- structural nears tried again and left: DrawDynamicParticles (pEffect
  in a register), CModelMgr::ForEach (_Inc inlined), MarkSectionBreaks
  (Lightmap_Texture), LoadLightGroup (push_front not inlined in the
  exe's copy), LoadRenderStyle (retail calls string's c_str out of line):
  each is an inlining or register decision no source form tried moves;
- nears left as register allocation or scheduling only (tried, no source
  shape found): d3d_DrawPolyGrid (one imul order; the expression and
  statement orders tried), MoveObject, RotateWorldModel, StairStep,
  Collide, QueueShadows, d3d_RenderWorldWithAggregate,
  r_LoadSystemTexture, CD3D_RenderWorld::Load and Draw, BuildDeviceList,
  CreateDevice, BuildFontTable, MoveAttachment, LoadPixelShader,
  SetObjectFlags, sm_AllocateID, SendAllObjectsUnguaranteed,
  SetTransformMode, ps_UpdateParticleBoundingBox;
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
- On master 5dd804d (the game's join fixes, engine round 7) with mptest's
  CD key: all three combinations join DE_Island and play (the client in
  the level with its HUD and weapon): ours against ours, retail's client
  DLLs on our engine against our server, ours against retail's server.
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

### 8b. ui (the visible differences from retail, outside the join)

Compared screen by screen under Wine (out/tour scripts for build/playtest.sh,
`mclick` on menu items) against the control: our engine with retail's game
DLLs.

- Fixed (branch `ui`): the main menu (no LAN item, no game type list, the
  version text "v1.1 - Retail" from CVersionMgr::GetDisplayVersion and the
  literal mod name "Retail"); the fonts' space width (SetDefCharWidth, so
  the briefing wraps as retail's); the multiplayer missions file, written
  by CGameClientShell at every start as MPMissions.txt (identical to
  retail's, DE_ worlds and SelectedWeapon included); the game options (no
  load screen tips) and performance screens (no Test Settings); the load
  screen (the mission's name for Contract J.A.C.K.'s blank level names,
  saves indented under their labels). Single player, options and its
  sub-screens, controls, profile, load and the in-game system menu now look
  as retail's; the HUD in M01S01 too.
- Not differences: "Press any key to continue." flashes between two colours
  in both (0.333 s, the colour XOR 0xFFFFFF); "LithTech build
  4294967295.4294967295" is retail's too (retail's Lithtech.exe has no
  RCDATA 12346 version resource either, so GetLTExeVersion fails in both).
- **The screen and command enums are not retail's.** Retail's screen names
  (CShell.dll .data 0x1014fed8, 32 bytes each) have no JOIN_LAN and no
  PLAYER_SKILLS and add HOST_DEMOLITION_OPTIONS after HOST_TDM_OPTIONS
  (retail: GAME = 14, ours 16). eScreenCmds needs one more entry before
  CMD_OPTIONS (added here, CMD_DEMOLITION: retail's options are 18), one
  more before CMD_EDIT_PASS (retail 0x28) and one more before
  CMD_CROSSHAIR (retail 0x44; CMD_CUSTOM retail 0x52). The screen enum
  change was made and measured: +37 functions, but `make progress` failed
  on one recorded function (sub_100c9030, a join callback that only
  matched our host screen's because the old ID coincided) and on CShell's
  unwritten count (175 to 192, ScreenJoinLAN and ScreenPlayerSkills
  leaving the project). Not committed: it needs ACCEPT_LOSSES or a
  coordinator's decision.
- **For the game package**: retail's multiplayer screen is Contract
  J.A.C.K.'s own (Player setup, CD Key, Join, Host; CShell.dll
  0x100d3xxx); ours is NOLF2's and crashes in CScreenMulti::Update (its
  WON server directory is NULL) once the main menu's Multiplayer reaches
  it. Retail's load screen also lists a checkpoint save
  (CClientSaveLoadMgr 0x100b5330, the INI key CheckpointSave, IDS 1444 and
  1445), which ours has no support for. ClientMultiplayerMgr's m_sModName
  is at 0x166c in retail, 0x1670 in ours.

### 8c. cshell (CShell.dll's Contract J.A.C.K. screens)

Round 1 (branch `cshell`), the items 8b leaves open:

- The screen and command enums are retail's: no JOIN_LAN, no
  PLAYER_SKILLS, HOST_DEMOLITION_OPTIONS after HOST_TDM_OPTIONS
  (ScreenEnum.h, the .data names at 0x1014fed8); eScreenCmds has three
  single-player commands after CMD_NEW_GAME that retail never handles
  (CMD_CJ_24..26: ScreenSingle::OnCommand's jump table, 0x100dc930, has
  LOAD_GAME at 27), no CMD_JOIN_LAN and no CMD_SKILLS, and CMD_TOGGLE_LAN
  after CMD_TOGGLE_SCMDPASS (the host screen's 45). Checked against every
  retail OnCommand jump table (single, save, host, host levels, join,
  options): CMD_PLAYER 37, CMD_EDIT_PASS 40, CMD_EDIT_CDKEY 57,
  CMD_DISPLAY 58, CMD_CROSSHAIR 68, CMD_CUSTOM 82. ScreenJoinLAN and
  ScreenPlayerSkills left the project (their retail matches became
  unwritten: CShell's unwritten count rose).
- CScreenMulti is retail's (0x100d3130..0x100d3cbb): Player setup, CD Key,
  Join, Host; the key typed in capitals (an edit filter) and grouped by
  fours; checked by the GameSpy browser's IsCDKeyValid and kept in
  CVersionMgr; an Escape back to single player's missions. All but
  HandleCallback match (the dash's register and its last return).
- CScreenHostDemolitionOptions (0x100c40d0..0x100c46a1) is new; the DM,
  TDM, DD and coop host options are retail's (the team items without
  their names, no tag score, no skills toggle, weapons stay, and an
  internet game's most players capped by the server's bandwidth). The
  team screens keep their unused team name pointers: retail's objects are
  that size (TO2ScreenMgr's debug_new_fn).
- CScreenHost is retail's (0x100c1ad0..0x100c37c3): a game type cycle
  (DM, TDM, Doomsday, Demolition; CMD_GAME saves and updates), a
  LAN/Internet toggle (m_bLAN, the profile's m_bLANOnly and g_bLAN), the
  bandwidth's column with the most players it allows, no load item, no
  Escape of its own, and the default campaign listing DE_ worlds for
  deathmatch and Demolition.
- ClientMultiplayerMgr has no WON server directory member (m_sModName at
  0x166c as retail's); GetServerDir() returns NULL.
- The checkpoint save: CSaveLoadMgr::CheckpointSaveExists and
  GetCheckpointSaveFile ("CheckpointSave", "Checkpoint.sav"),
  kLoadGameTypeCheckpoint (3, so kLoadGameTypeContinue is 4),
  CClientSaveLoadMgr::LoadCheckpoint, CMissionMgr::StartGameFromCheckpoint
  (eStartGameFromCheckpoint 5) and the load screen's entry (CMD_CUSTOM+2;
  the slots now start at CMD_CUSTOM+3). **For the game package**: the
  server (CServerSaveLoadMgr::HandleLoadGame) has no checkpoint case yet
  and nothing writes the checkpoint save (Object.lto has the strings
  "CheckpointSave" and "Checkpoint"); until then the entry never shows.
- The auto targeting (aim assist) is Contract J.A.C.K.'s: CAutoTargetMgr
  (0x10003770..0x10004811, FEAR's AutoTargetMgr is its descendant) updated
  by CPlayerMgr::Update after the lean, aiming GetFireVectors' first
  person shots once locked on; CAimMagnetFX (0x10001e70..0x10001f3a, its
  SFX_AIMMAGNET_ID bank and message); the skeletons' Node%dAutoTarget
  (CModelButeMgr's node 0xb9, at most eight a skeleton) and
  COMMAND_ID_MANUALAIM toggling it off. A weapon's UseAutoTarget decides:
  0 never, 1 on easy, 2 always. CHUDCrosshair::Render is retail's
  (0x10055620): once locked on, the crosshair is drawn at GetCrosshairPos
  with a dot left at the centre (behaviour; the block order still
  differs). Left: the vehicle weapon's aim (0x10004682), CPlayerMgr's
  m_bManualAim at its retail offset (0x1cd; ours is the last member), and
  **for the game package** the server's AimMagnet objects (Object.lto
  has AimMagnet and CAimMagnetPlugin) that create the magnets.
- Mouse look (8d's finding) is not a game code fault. Instrumented runs
  (CPlayerMgr::CalculateCameraRotation logging the axis offsets) show
  ours pitches and turns as retail: in M01S01 the player starts seated on
  a PlayerLure (PPM_LURE, camera limited to 40 up and 55 down), in ours
  and in the control alike (their shots after `turn 0 200` and `turn 0
  400` are the same), and in M03S02 the turned and look-down shots match
  the control's. What differs between runs is the harness: Wine's
  pointer re-centring sometimes reports a step back (negative x offsets
  between xdotool's 40-pixel steps), so a 400-pixel turn lands short in
  some runs of either game. Smaller, slower steps (`turn 400 0 40`)
  should make the scripted runs comparable.
- CScreenJoin is retail's GameSpy browser (0x100c8f40..0x100d06b0), in
  behaviour: its states (Startup, UpdateDir, QueryDetails, Waiting), the
  search order (LAN on the retail and the demo browser, then the
  Internet on both while it answers), the LAN port column (the profile's
  client port), filters (version, players, ping, game type), six sort
  columns (name, type, ping, players, lock, version), the details window
  (mission, options parsed from the "options" key, players) and the
  password prompt. Retail keeps the servers in a hash_map and makes
  their rows on the browser's thread; ours keeps a std::map and hands
  the servers to the screen's update through a locked queue, so neither
  thread waits on the other's lock. The sort instantiations stay
  unwritten.
- ClientMultiplayerMgr joins as retail: SetupClient with the server's
  public and private addresses (0x1001f2c0), StartClient's GameSpy NAT
  negotiation on the engine's socket (0x1001ed10), the join states run
  from Update (0x1001f670: the NAT wait, the two seconds' grace, then
  JoinServer trying our address, the private one, the public one). Ours
  connects at once when there is no NAT negotiation (retail waits for
  the next update). CTO2GameClientShell::ProcessPacket hands packets to
  the browser (0x100fa150); CInterfaceMgr::LoadFailed shows "server
  full" for LT_REJECTED (0x10063960). Shared/NetDefs.cpp gained
  "Demolition" (as the game branch's).
- Tested by hand (a scratch copy of mptest.sh clicking through the
  menus on a private docker network): the server is listed, its details
  show, and Join game enters it -- **once the server shares its socket
  with GameSpy**: Object.lto's GameSpy startup (0x100dfa33) takes
  g_pLTServer->GetUDPSocket (ILTServer vtable 0x118) into
  StartupInfo::m_hSocket and fails without it; ours passes 0, so
  GameSpy answers on port+1 and the join goes there. That is the game
  package's (ObjectDLL/GameServerShell.cpp). Other agents' test servers
  on the default docker bridge show in the LAN list.
- Unwritten rows, round 2 (61 -> 32): rows the enumerator split inside a
  function are in progress/CShell-screen-exclude.csv (each cites its
  container) and missed starts in extent-splits.csv; functions retail
  keeps though nothing calls them are linked in with /include pragmas
  (GameClientShell.cpp). Written for retail's behaviour: PlayerMgr::Term
  letting go of the last messages, the respawn keeping the last
  orientation, InitTargetMgr making CAutoTargetMgr; the loading screen's
  SetDefaultMissionNames; OnCommandOff's retail cases; the mouse buttons
  ending the movies after the first run; ConnectionFailed and LoadFailed
  as retail's; StartingNewGame clearing the join screen and the patch
  check; SetupMenuMusic after a disconnection; CMusic::Play/Stop;
  SetNewLevel no longer going to the loading state (its callers do,
  SetupSavedLevel for saves); +join's +publicserver; LauncherServerApp
  starting ContractJackServer.exe; SetupServerHost's Demolition case and
  bandwidth limit; CSnowFX's on/off message (m_bFromTop is not used yet).
- Round 3 (32 -> 21): the weapon's heat per shot and the recoil as the
  firing perturb; IsCarryingHeavyObject from the chassis attributes; the
  lean ray's filter; CWeaponFX::CreateBeamFX; SharedMovement in the
  client ("Vehicle", as retail's tables); the player setup screen's
  deathmatch model in every game type (the playtest's missing 3D model).
  CCharacterFX reads the pitch from the object's unguaranteed data
  (UpdatePitch -> HandlePitch, retail 0x10012820/0x10012680); the old
  CFX_PITCH message still works until the server drops it.
- Where it stops: 21 unwritten. Left: about 10 empty stubs our linker's
  ICF folds (NOICF cost 184 unplaced, reverted), size-pairing leftovers
  in GameClientShell.obj, ClientMultiplayerMgr.obj and ClientWeapon.obj,
  VehicleMgr's (the game package's), CMissionMgr's constructor (retail's
  world names are std::string), a ScreenJoin sort helper, and
  ScreenHostWeapons' Escape (retail's screen works differently). The
  CD-key dialog's Return could not be reproduced in a scripted run (the
  dialog closes, nothing behind it acts); mouse look is as in 8d's
  finding.

- Mouse look, measured (M03S02, `turn 400 0 40` then `turn 0 300`):
  the client's whole path is retail's (CalculateCameraRotation's maths,
  the profile's Sensitivity 2 and InputRate 12, ImplementMouseSensitivity's
  `scale` 0.0035, UseCursor/CursorCenter, the adaptive mouse off). Logging
  the axis offsets per frame: our CShell turned 1.72 rad with our Object
  and 3.28 rad with retail's Object.lto, with no jumps between frames;
  the offsets themselves add up to less. The difference is the frame
  rate: during the turn our game runs 39-45 fps against 48-75 with
  retail's Object (afterwards about 140 against 170), and at fewer frames
  more of the harness's pointer motion is lost to Wine's re-centring
  (negative offsets: 16 against 5). Pitch moves down and clamps as
  retail's. So the fix is our Object.lto's per-frame cost on the server
  side, not CShell's input code.

- Round 4 (21 -> 9): CMissionMgr's world names as std::string, fresh
  StartGameRequests, DeleteGameSpyBrowsers for the host, the multiplayer
  update under the menus (CPlayerMgr::UpdateUnderMenus,
  CGameClientShell::UpdateEffectsNoRender), the time bomb's
  SpecialOverrideFire on the target manager's object (0x10025760), and
  the merged runs of stubs split. Where it stops: 9 unwritten -- four are
  VehicleMgr's (the game package's), CreateObject's ICF-folded copy
  (0x10095e60), and size-pairing leftovers (ClientMultiplayerMgr's
  destructor 0x10020150 against ours of 160 bytes, OnCommandOff
  0x10063ee0, SetupSavedLevel 0x10088810). Next: those, then the
  unplaced functions.

### 8d. playtest coverage (branch `apps2`)

Harness: `make control` (out/cjack-control: retail's game with our
engine), `make campaign` (build/campaign.sh: every single-player world,
build/playtest-level.txt: into control, move, look, fire, quicksave,
quickload; CAMPAIGN_CONTROL=cjack-control compares each world's shots),
`make mptest MPTEST_TYPE=dm|tdm|dd|de` (the first world of each game type;
MPTEST_WINEDBG=1 for a crash backtrace). Run on master ae759b5 + `apps2`.

| What | Ours | Control | Finding, owner |
|---|---|---|---|
| Single player, all 15 worlds (M01S01 ... M07S04, EPILOGUE) | all load, play, save and load; no crash | same | (harness: a run can be judged NO HUD when skipcine's keys land before the level, 1 in 3 for our M01S01, and the control's M04S02/M04S03 post-load check; rerun) |
| Mouse look | turn 400 px turns noticeably less than retail; turn 0 200 (look down) does not pitch at all | pitches down to the floor | **cshell**: mouse sensitivity / vertical look (CPlayerMgr's mouse input, profile Sensitivity/InputRate). It makes every later shot of a scripted run diverge from the control. |
| Firing (M03S02, standing) | fires, muzzle flash, ammo counts | same | none |
| M06S01 (space) | no weapon in hand after the moves; no "Laser Rifle" pickup message | picks up the Laser Rifle on the same path | **game/cshell**: probably the diverging movement above; recheck once mouse look is fixed |
| Load after quickload, M01S01 | no checkpoint | "Saving checkpoint..." with its icon after the load | **cshell/game**: Contract J.A.C.K.'s checkpoint saves (known) |
| Multiplayer deathmatch, team deathmatch | join, play on DM_Khios | same | none |
| Multiplayer Doomsday (DD_AntarcticAggression) | client crashes once in the world: read of 0xDA in TVector3<float>::operator== (Lithtech.exe 0x40c710, an ICF-folded helper reached through clientde_impl) right after MID_PI_UPDATE | joins and plays | **cshell/game**: our game code passes a bad vector pointer to the engine (the control, same engine, is fine); a backtrace needs MPTEST_WINEDBG with winedbg's console output, not yet captured |
| Multiplayer Demolition (DE_Bollywood) | server: "Unable to load the game binaries" (no Demolition game type: no CDemolitionMissionMgr, NetDefs' type strings end at DoomsDay) | joins and plays | **game**: retail Object.lto has CDemolitionMissionMgr and DemolitionGameOptions; CShell.dll ScreenHostDemolitionOptions |
| Vehicles (M03S01, M03S02, M07S02 have PlayerVehicles) | not reached | not reached | harness: blind movement does not find them; needs a save at a vehicle (PLAYTEST_SAVES) |
| Modern build | single-player script and mptest pass | -- | none |

Stopped here: the harness is committed; next, a backtrace of the Doomsday
crash and a vehicle save to start from.

**Re-run on master 527e543 and f48248e** (branch `apps3`):

| What | Ours | Control | Finding, owner |
|---|---|---|---|
| Single player, all 15 worlds | all load, play, save and load, no crash | same (M04S03 once NO HUD, the harness) | screenshots still diverge after the first turn (mouse look) |
| Multiplayer dm, tdm, dd | join and play (Doomsday no longer crashes, here or for the game agent) | same | none |
| Multiplayer Demolition (DE_Bollywood) | joins and plays (f48248e) | same | **cshell/game**: no round timer at the top of the screen (the control shows 03:04) |
| Team setup screen (tdm, de) | never shown here: the client is put straight into the world | same | none here; the team pick is not scripted (see the report) |
| Checkpoint save (build/playtest-checkpoint.txt, M01S01's corridor) | no Checkpoint.sav, no "Checkpoint" row on the load screen (527e543) | "Saving checkpoint...", Checkpoint.sav, CheckpointSave in Save1001.ini, a "Checkpoint" row on the load screen | **game/cshell** (the server half is in f48248e: re-run) |
| Menus (build/playtest-menus-cj.txt, world "-") | single player, difficulty, profile and its dialogs, quit: as the control | -- | none |
| Multiplayer screen after the CD key dialog's OK | Return goes on into Player setup | stays on Multiplayer | **cshell**: the dialog's Return also reaches the screen |
| Player setup | no 3D player model; a click cycled Bandwidth to T1 (1500) | the model shown beside the items | **cshell** |
| Join screen | crash: NULL read at CScreenJoin::Update (CShell.dll +0xbf300 in our map) | the server browser | **cshell** |
| Vehicles | Teleport x y z is a console program (CShell registers it) but neither the legacy nor the modern engine opens a console here (nolf2src's modern console, M-E5, is not in cjacksrc); the PlayerVehicles are SnowmobileStart_01 (528, 429, -414) in M03S01, SnowmobileDummy_01 (9255, -298, -1912) in M03S02, Vespa01 (-1907, 701, -13017), Vespa02 (4312, 994, -5040) and PlayerVehicle03 (-3340, 839, 5600) in M07S02 | -- | **engine**: a developer console (M-E5) would let the harness teleport to them |

mptest's MPTEST_WINEDBG now writes a minidump through AeDebug and reads
its backtrace (crash.txt); it has not met a crash since (Doomsday no
longer crashes).

**Re-run on master 43d3544** (apps3, merged as 3eec59b):

| What | Ours | Control | Finding, owner |
|---|---|---|---|
| Single player, all 15 worlds | all load, play, save and load, no crash | same | screenshots still diverge after the first turn |
| Mouse look | still: a 400 px turn turns less than retail's; a 200 px pitch does not look down | turns further, looks down | **cshell** |
| Multiplayer dm, tdm, dd, de | all four join and play; no crash (Doomsday included) | same | none |
| Demolition HUD | no round timer | the timer at the top (02:32) | **cshell/game** |
| Team setup (tdm, de) | not shown: the client goes straight into the world, ours and the control alike | same | none here; no team pick scripted, as nothing asks for one in these runs |
| Checkpoint save (build/playtest-checkpoint.txt) | written: Checkpoint.sav, CheckpointSave in Save1001.ini, "Checkpoint" on the load screen | same | fixed (game, f48248e) |
| Join screen (build/playtest-menus-cj.txt) | crash: NULL read at CScreenJoin::Update+0 (CShell.dll loaded at 0x018B0000; 0x100bf1a0 in our map) | the server browser | **cshell** |
| Multiplayer after the CD key dialog; Player setup | Return goes on into Player setup; no player model | stays on Multiplayer; the model shown | **cshell** |
| Vehicles | unreachable: no console to Teleport to them (see above) | -- | **engine** (a developer console) |

**Unplaced functions (ours with no shipped function placed), where the
apps agent stopped** (master 9384c60 merged into `apps3`, the rebuild
stopped part way: rebuild before `make progress`). The coordinator's
decision: record per function, with evidence, in progress/ (a
`<target>-ours-exclude.csv` and, if needed, a small neutral hook in
progress.py on tools a22f499), and fix the real ones first.

| Image | Unplaced | What they are |
|---|---|---|
| ContractJackServer.exe | 47 | all ButeMgr.obj's exception-handling helpers the compiler emits (`__ehhandler$...`, `$L...` unwind stubs) |
| ClientFx.fxd | 48 | 30 clientfx.obj exit-time destructor pairs for its global banks (`_$E23`..`_$E66`: `push; call atexit` and `mov ecx, bank; jmp Term`; retail has as many of both shapes, 17 each, so likely the shape count against retail's function table); 11 initializers (ltmodule.obj, stdafx.obj, 19 bytes); 5 exception-handler helpers (lightningfx.obj); 2 real: CPlaySoundFX::SuspendedUpdate and CPlayRandomSoundFX::SuspendedUpdate (64 bytes, the same body: retail may fold them or not override) |
| GameSpy.dll | 13 | 11 exception-handler helpers; NatNegProgressCallback and NatNegCompletedCallback (one `ret` each) |
| SndDrv.dll | 33 | initializers and exception-handler helpers; real: CounterFinal::CountMS, LTMemTerm, CSample::~CSample, WaveFile::~WaveFile |
| server.dll | 475 | not gone through (world_blind_object_data.obj 14, s_client.obj 11, serverde_impl.obj 10, ...) |
| Lithtech.exe | 1,392 | not gone through (d3d_renderworld.obj 172, ...); the engine agent: mostly math_phys.h initializers |

Next: for each real one, whether retail has it (inlined, folded, under
another pairing) or not, then the exclusion file for the compiler's own.

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
- Releases, as nolfsrc's and nolf2src's: a tag `v*` makes a GitHub release
  with the modern build's public archives (`cjacksrc-<tag>.tar.gz`,
  `cjacksrc-debug-<tag>.tar.gz`), and each push to master whose builds pass
  moves the pre-release `nightly` to it, with its archives
  (`cjacksrc-nightly-<commit>.tar.gz`); text from `.github/release-notes.md`
  and `nightly-notes.md`. The release step was dry-run with a stand-in `gh`
  (new and existing tag, nightly new, moved and stale).
- `mac-install.command` (Perl, `build/game-scripts/`; `tools/mkinstall.py`
  appends the same list as linux-install.sh's): checked on Linux to write
  the same 18 files as linux-install.sh, matching the private directory,
  from the European `.iso`s, from the USA `.zip`s found beside the folder,
  and from a USA `.cue`/`.bin` with the other disc's `.iso`; not yet run on
  a Mac.
- To do: `linux-run.sh` (and a launcher script) in the game directory once
  the engine runs, and the Windows install script tested under PowerShell.
