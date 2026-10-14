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
| 3 | `small`: `ltmsg.dll`, `CRes.dll`, `SRes.dll` (resource scripts from the retail resources; `tools/rsrcdiff.py`), `SndDrv.dll` (nolf2src's reconstruction, compiled with VC .NET 2003) | 2 | `ltmsg.dll` done (retail's but for its time stamp); `SndDrv.dll` builds, 172/260 identical; CRes/SRes to do |
| 4 | `game`: `CShell.dll`, `Object.lto`, `ClientFx.fxd` from NOLF2's source release toward Contract J.A.C.K. -- first the shared code (with Build 69's TO2/TRON as the later reference), then Contract J.A.C.K.'s own (its weapons, its missions' objects, its HUD and menus), placed by the scoreboard | 2 | |
| 5 | `gamespy`: `GameSpy.dll`: Monolith's wrapper from the image, GameSpy's SDK rolled back to 2003 | 2 | |
| 6 | `engine`: `Lithtech.exe` and `server.dll` from the Jupiter tree, trimmed to 2003, D3D8; the demos' engine for the SecuROM pages and the imports (a `demoimports.py` for these demos) | 2 | in progress: both build and are scored; converging (below) |
| 7 | `apps`: the launcher `ContractJack.exe` and the dedicated server `ContractJackServer.exe` from NOLF2's Launcher and ServerApp | 2 | |
| 8 | `boot`: the built binaries in the game under Wine, to the menu, a level, saving and loading (`make test`, tools/playtest.sh) | 3-7 | |
| 9 | `match`: byte-matching, module by module, ratcheted by `make progress` | 3-7 | |
| 10 | `modern`: the modern build (`CJACK_MODERN`): fixes (`RETAIL_BUGS.md`), widescreen, our Bink player on FFmpeg, as nolf2src's | 8 | |
| 11 | `release`: `-private`/`-public` archives, install scripts from the player's own discs and 1.1 update (`tools/mkinstall.py`), and the GitHub Actions workflow building the four public archives with `make progress` on the legacy release | 8, 10 | |

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
- `SndDrv.dll`: nolf2src's reconstruction (NOLF2's driver), with the one
  change VC .NET 2003's library needs (`push_back` takes an argument, as
  the 2006 tree has it), and without STLport: Contract J.A.C.K.'s imports
  MSVCP71 (`vector`'s length errors build a `std::string`). 172 of 260
  functions identical as it stands; the 2006 tree's
  `runtime/sound/src/sys/s_dx8` is the later version to compare the rest
  against. Our image is 53,248 bytes against retail's 57,344.

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
CClientMgr's 2006-tree members and resource-tree vector. Score so far:
`Lithtech.exe` 6,020 identical (71.8%), `server.dll` 2,492 (79.1%).

**Where it stands / next** (breadth first, per module, with
`tools/fndiff.py`, `o2gap.py`, `vtabcheck.py`, `uncovered.py`;
`tools/layoutscan.py --image=<target>` finds the layout shifts):
- the UDP driver: CJ's is between nolf2src's 2002 form and the 2006
  tree's: four listen-thread events (the 2006 pause events), a fifth
  CStaticFIFO<float,100> in CUDPConn (+0x2a8: the share of unguaranteed
  packets dropped as too old, averaged by a new CBaseConn virtual, slot
  9), a new CBaseDriver virtual (slot 27, before UpdateGUID), OpenSocket;
- the unguaranteed-data API (ILTServer/ILTClient Get/SetObjectUnguaranteed-
  Data, the 0x80000 change flag on the wire, CLTServer::FinishClientConnect);
- the renderer's layouts: CRenderShader_Base (+8), CInternalSection (+4),
  CD3D_RenderWorld, CD3D_Device;
- then module by module through the nears (`progress.py --nears=<target>`).
