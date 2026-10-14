# What is whose

Contract J.A.C.K.'s source was never released. What is here is either
Monolith Productions' code from the sources it did release (NOLF2's game
code, the LithTech Jupiter engine), brought to what the retail Contract
J.A.C.K. binaries are, or reconstructed from those binaries -- Monolith's
code in substance either way.

| Path | Whose | Terms |
|---|---|---|
| `src/Engine/sdk/`, `src/libs/` | **Monolith Productions**: the LithTech Jupiter SDK headers and Monolith's shared libraries, as the *No One Lives Forever 2* source code release (version 1.3) carries them (by way of nolf2src), changed where Contract J.A.C.K. differs | Monolith's terms for that release; `src/Engine/sdk/Legal.txt` is its legal notice. Work out those terms before distributing anything built from it. |
| `src/Engine/snddrv/`, `src/libs/ltmem/` | nolf2src's reconstruction of NOLF2's sound driver `SndDrv.dll` (and of what it links of `ltmem`), brought to Contract J.A.C.K.'s: **Monolith Productions'** code in substance. `eax.h` and `eaxguid/` restate what the driver uses of **Creative Labs'** EAX 2.0 interface | as Monolith's code above |
| `src/Engine/ltmsg/` | `ltmsg.rc` is a reconstruction of the retail `ltmsg.dll`'s resources (Monolith's engine messages, the same as NOLF2's) | as Monolith's code above |
| `src/Game/Libs/GameSpy/*.cpp`, `*.h` | **Monolith Productions**' `Game\Libs\GameSpy`, the wrapper `GameSpy.dll` is: no source of it was ever released; reconstructed from the retail `GameSpy.dll` (1.1) so that it compiles back to it. `WinSync.h` is Monolith's synchronisation classes from the LithTech Jupiter tree (`runtime/shared/src/sys/win/winsync.h`), which the retail code shows the wrapper used | as Monolith's code above |
| `src/Game/Libs/GameSpy/GameSpySDK/` | **GameSpy Industries / IGN Entertainment**: the GameSpy SDK's C source as IGN released it in 2011 under a BSD licence, from the snapshot mirrored at github.com/nitrocaster/GameSpy (commit `d1deb2d1a951`; serverbrowsing 2.07.00, December 2007). Only the modules `GameSpy.dll` links are kept, each with its `changelog.txt`; they are rolled back toward the July-October 2003 versions the retail image was built from, and the 2007 `common/` layer is replaced by a 2003-style `nonport.h`/`nonport.c`, written for this project | the three-clause BSD licence in `GameSpySDK/LICENSE.txt` (copyright IGN Entertainment, 2011): keep the notice with the source and reproduce it with binaries |
| `src/Game/Libs/WONAPI/` | the part of **WON**'s client library (WONAPI, as Monolith's tree had it in `Game\Libs\WONAPI`) that `GameSpy.dll` links for its CD key check (`WONAPI::CDKey`, `CRC16`, `ByteBuffer`), written for this project from the retail image; WONAPI's own source is not in this repository | written for this project; no licence is declared |
| `src/cjacksrc/`, `build/`, `docker/`, `Makefile`, the documentation, `progress/` | written for this project (much of it adapted from nolfsrc and nolf2src) | no licence is declared |
| `rez/` (submodule) | the disc images' installer; the images themselves are fetched from archive.org | see `rez/README.md` |
| `toolchains/` (submodule) | the compilers, assembled from public copies of the original media | see `toolchains/README.md` |
| `tools/` (submodule) | the reconstruction tools, written for these projects | see `tools/README.md` |

The game code (`src/Game/`, from NOLF2's source release) and the engine
(`src/Engine/runtime/`, from the LithTech Jupiter engine source) are being
brought in by the packages of `docs/PLAN.md`; each adds its row here when
it lands.

What a build puts in a game directory, by origin (`MANIFEST.txt` in the
directory lists every file; `tools/retailextract.py audit` every binary):

| | Whose | From |
|---|---|---|
| everything built (`MANIFEST.txt`: `built`) | built here from the source above | `src/` |
| the game data: `.rez` archives, movies, configs | Monolith Productions / Vivendi Universal Games | the retail game (the retail game DLLs inside the archives are zeroed) |
| `binkw32.dll` (Bink), the Microsoft C++ and MFC 7.1 runtime DLLs | RAD Game Tools, Microsoft | the retail game |

## The archives

Each game directory is packaged twice (`build/package.sh`; `BUILDING.md`).
This says what each holds; it is not legal advice and grants nothing.

**`<directory>-private.tar.gz`** is the whole game directory, the retail
game's data and the third-party runtime included. Those are Monolith
Productions' and Vivendi Universal Games', RAD Game Tools' and Microsoft's,
under the terms the retail game came with: the private archive is for your
own machines, and for others developing on the build who have the game, not
for handing out.

**`<directory>-public.tar.gz`** holds only what the build made and the
project's own files: the binaries built here (`.pdb` and `.map` too, for
the debug builds), `linux-install.sh`, `windows-install.bat`,
`cjackcmds.txt` and `MANIFEST.txt`. It has no `.rez` archive, movie,
setting or profile of the retail game, no RAD Bink and no Microsoft runtime
DLL. The install scripts copy those from the player's own disc images of
Contract J.A.C.K. (the European or the USA release) and its English 1.1
update into the directory, on the player's machine; nothing of them is in
the archive, and the scripts carry no InstallShield or Microsoft code (they
inflate with python3's zlib or .NET's DeflateStream). So the public archive
is the one meant for other people -- but its binaries are built from
Monolith's code, whose terms no licence here overrides: work them out
before distributing it. The GitHub Actions workflow
(`.github/workflows/public-archives.yml`) makes only public archives.
