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
**SDK change for the game:** `ltbasedefs.h`'s LTStrCat is `__forceinline`
only under DIRECTENGINE_COMPILE or __D3DREND (the engine's projects); the
game's code still sees the plain inline.

**Where it stands / next** (stopped here: score recorded at
`Lithtech.exe` 6,713 identical; `server.dll` 2,717, 95.2%;
SndDrv.dll 225) (breadth first, per module, with `tools/fndiff.py`,
`o2gap.py`, `vtabcheck.py`, `uncovered.py`; `progress.py --nears=<target>`
pairs some rows wrongly where a function's CSV extent is split, so check
each with fndiff against the retail address first):
- nears left as register allocation only (tried, no source shape found):
  CD3D_RenderWorld::Draw, CPlayerMover::Collide/_Poly/StairStep,
  MoveObject, RotateWorldModel, QueueShadows, LoadLightGroup, SSubLM::Load,
  the light-group stream operator, CPixelShaderMgr::FreeShader;
- RunClientApp, InstallPixelShader and a few others call through
  SecuROM's API thunks in the demo image (`call 0x612bf0` plus a junk
  byte): `tools/demoimports.py` should resolve those six-byte calls too;
- d3d_BlitToScreen3D (989 bytes against 1,085; d3d_BlitToScreen no longer
  calls the _Old form, which is now unused): CJ's tile clipping loop;
- d3d_utils (d3d_GetColorMasks, d3d_D3DFormatToString), BuildDeviceList,
  ps_UpdateParticleBoundingBox, DrawDynamicParticles, con_WriteHistory,
  cc_SaveConfigFile, CClientShell::Term, CSoundMgr::Term, df_FindNext,
  input_TrackDevice, GetFrameOccluders, AllocShader,
  d3d_RenderWorldWithAggregate, COccludee::SplitPlane;
- server: SendAllObjectsUnguaranteed, FillPacketFromInfo,
  UpdateSendTimeWithAttachments, CServerEvent::DecrementRefCount;
- client.cpp keeps nolf2src's NOLF2SRC_MODERN blocks (never compiled
  here): to become CJACK_MODERN when the modern build is taken up;
- the two Lightmap_Texture-style SortSections were fixed by dropping the
  textured lightmap sections' constructors; watch the modern build for
  code that relied on them.

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
