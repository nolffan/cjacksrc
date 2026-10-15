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
| 6 | `engine`: `Lithtech.exe` and `server.dll` from the Jupiter tree, trimmed to 2003, D3D8; the demos' engine for the SecuROM pages and the imports (a `demoimports.py` for these demos) | 2 | |
| 7 | `apps`: the launcher `ContractJack.exe` and the dedicated server `ContractJackServer.exe` from NOLF2's Launcher and ServerApp | 2 | |
| 8 | `boot`: the built binaries in the game under Wine, to the menu, a level, saving and loading (`make test`, tools/playtest.sh) | 3-7 | |
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

The game DLLs build (`src/Game/TO2.dsw`); the launcher and the dedicated
server are not in the workspace yet (their resources first).

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
- **Flags** (evidence in `progress/COMPILERS.md`): NOLF2's `/MD /O2 /GX`,
  with `/GR` in the TO2 projects as well as the shared ones, `/GS` in
  LTGUIMgr only.
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
  (0x100dbcb0), the socket the engine hands GameSpy (ILTServer vtable
  0x118, missing from our SDK), CDemolitionMissionMgr, and the client side
  (IGameSpyBrowser in ClientMultiplayerMgr and the join screens,
  IGameSpyPatchInfo); the client still has NOLF2's WON code, with no
  directory created.
- **Scores**: CShell.dll 3,602 identical of 6,567, Object.lto 5,848 of
  9,787 (811 KB), ClientFx.fxd 285 of 452. The runtime boundaries are
  pinned from our maps.
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
  CVersionMgr. CClientWeapon has 79 virtual functions to our 66 (many of
  NOLF2's non-virtual ones made virtual, for CVehicleWeapon).
- **Layouts still to change** (from the constructors): CAnimatorPlayer
  (+4 from about 0xc28, 28 bytes in all: more animation players, it
  seems), which moves CPlayerObj's members from 0x2638 by 28; KeyFramer's
  other HSTRINGs are still HSTRINGs in retail.
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
