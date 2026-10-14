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
| 2 | `build`: `build/env.sh`, `build/build.mk` and `build/compile.sh` for VC .NET 2003 under Wine, the workspace `src/Game/TO2.dsw`, `build/all.sh` assembling a game directory, `build/variant.sh` (debug, modern) | 1 | |
| 3 | `small`: `ltmsg.dll`, `CRes.dll`, `SRes.dll` (resource scripts from the retail resources; `tools/rsrcdiff.py`), `SndDrv.dll` (nolf2src's reconstruction, compiled with VC .NET 2003) | 2 | |
| 4 | `game`: `CShell.dll`, `Object.lto`, `ClientFx.fxd` from NOLF2's source release toward Contract J.A.C.K. -- first the shared code (with Build 69's TO2/TRON as the later reference), then Contract J.A.C.K.'s own (its weapons, its missions' objects, its HUD and menus), placed by the scoreboard | 2 | |
| 5 | `gamespy`: `GameSpy.dll`: Monolith's wrapper from the image, GameSpy's SDK rolled back to 2003 | 2 | |
| 6 | `engine`: `Lithtech.exe` and `server.dll` from the Jupiter tree, trimmed to 2003, D3D8; the demos' engine for the SecuROM pages and the imports (a `demoimports.py` for these demos) | 2 | |
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
