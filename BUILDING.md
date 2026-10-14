# Building

`make` is the whole build. This file describes what it does, how to run
parts of it by hand, and how the game is packaged.

## What you need

* **Linux with Docker.** The compilers are Win32 programs and run under Wine
  in a container; the host needs nothing else (no Wine, no Python).
* **The submodules**: `git submodule update --init`.
  * `toolchains/`, the compilers: Visual C++ .NET 2003 (13.10.3077, the
    compiler every retail binary names), the DirectX 8.1 SDK, and VC6's
    for the one object of `SndDrv.dll` it built. See `toolchains/README.md`.
  * `rez/`, the retail game the build takes its data from. It fetches the
    redump images of the two European discs of *Contract J.A.C.K.* from
    archive.org, checks them, and installs the game from them as its own
    setup would, then the English 1.1 update, into
    `rez/games/contract-jack-eu-1.1/`, checked file by file against
    `rez/games.sha256`; and the USA release's two discs, which the public
    archive's install scripts are made against too (below). `make` does it
    the first time (`make retail`); see `rez/README.md`.
  * `tools/`, the reconstruction tools, shared with nolfsrc and nolf2src.
* About 10 GB of disk: 3.4 GB for the docker image, 3.5 GB in `rez/` (the
  disc images of both releases, zipped and unzipped, and the installed
  game), and a game directory in `out/` for each build (the retail files
  the build does not change are hard links to `rez/games/`), with its build
  tree, Wine prefix and archives.

## What `make` does

1. Checks the compilers are in `toolchains/` and that `rez/` and `tools/`
   are there.
2. Builds the docker image `cjacksrc-build` from `docker/Dockerfile` if
   needed (a few minutes, once).
3. `make retail`, in a container: the first time, fetch the disc images and
   install them (`build/retail.sh`); every time, check the installed files.
4. Runs `build/all.sh` in a throwaway container (`--rm --init`, named
   `cjacksrc-build`), as your user, with the checkout (and so `rez/`)
   mounted at `/repo`.

`build/all.sh` then:

| Step | What | Output |
|---|---|---|
| 1. resources | the game data from the retail game: the `.rez` archives (the retail game DLLs inside them zeroed), the movies, the shipped `autoexec.cfg` and default profile (`build/retail.py` says which) | `out/cjack-legacy-release/` |
| 2. third-party runtime | Bink (`binkw32.dll`) and the Microsoft C++ and MFC 7.1 runtime DLLs from the retail game: licensed binaries with no source | `out/cjack-legacy-release/` |
| 3. compile | `build/compile.sh`: every compile and link, by `build/build.mk`, of every workspace in the tree (`WORKSPACES`: `ltmsg`, `snddrv`, `GameSpy`, the game's `TO2.dsw`, the engine's `Engine.dsw`) | `src/*/built/` |
| 4. assemble | the built binaries into the game directory; the game DLLs go in `custom/`, last in the rez stack (`cjackcmds.txt`). A binary whose source is not here yet is left out and named | `out/cjack-legacy-release/` |
| 5. audit | every PE file in the game directory listed with its origin (`MANIFEST.txt`); the build fails if one is unaccounted for, or if a `.rez` archive still holds one | |
| 6. package | the game directory as a compressed tar, whole (`-private`), and without the retail game's files plus the scripts that install them from the player's own disc images and 1.1 update (`-public`; below) | `out/cjack-legacy-release-private.tar.gz`, `-public.tar.gz` |

## Other targets

```sh
make progress     # score the build against the retail binaries, function by
                  #   function; fails if a function recorded as byte-identical
                  #   in progress/identical.txt no longer is
make progress-update   # record the score in progress/ and README.md
make reference    # the retail binaries and their function tables, in out/reference/
make shell        # a shell in the build container, at /repo
make clean        # remove out/ and every build output in the source tree
make distclean    # clean, and remove the docker image
```

`make progress` scores what the last `make` built (it compiles nothing); the
retail images it compares against are extracted into `out/reference/` the
first time. `progress/README.md` describes the scoring.

For the one-object loop of reconstruction, `build/env.sh` gives `run_cl`,
`run_link`, `run_lib` and `run_rc` (the build's own tools and environment;
`tools/guides/`).

## Variant builds: debug and modern

`make` builds the legacy release game, the only build `make progress` ever
scores. `make debug` (`CJACK_DEBUG`: symbols, logging), `make modern`
(`CJACK_MODERN`: the fixes and features of the modern build) and `make
modern-debug` (both) build the same source with those defines into
`out/cjack-legacy-debug/`, `out/cjack-modern-release/` and
`out/cjack-modern-debug/`. Each is `build/variant.sh <variant>`: the source
is copied into `out/build-<variant>/` and built there by the same
`build/all.sh`, so the release build's outputs are never touched; its
header has the details. The release build compiles the source with its
`CJACK_MODERN`/`CJACK_DEBUG` blocks resolved away (`tools/releasesrc.py`).

## Archives: private and public

Every game directory is packaged twice by `build/package.sh`:

* `out/<directory>-private.tar.gz` (about 0.8 GB): the whole directory,
  the retail game's data included. For your own machines, and for others
  developing on the build.
* `out/<directory>-public.tar.gz`: the same without anything that came
  from the retail game -- every file `MANIFEST.txt` lists as `data` or
  `THIRD-PARTY` -- and with two scripts that install those files from the
  player's own disc images of *Contract J.A.C.K.* (the European or the USA
  release) and its English 1.1 update, `contractjack_update_en_10_11.exe`:
  `linux-install.sh` and `windows-install.bat` (`tools/mkinstall.py`, with
  the layout in `build/retail.py`). They check every file they write
  against its SHA-256, so the result is the private archive's directory,
  byte for byte. The one to give to other people.

`LICENSE-NOTES.md` says what each holds and under whose terms.

Nothing is installed or run from the discs or the update. Contract
J.A.C.K.'s setup stores its files compressed in InstallShield 6 cabinets
that span both discs (`data1.hdr`, `data1.cab` and `data2.cab` on disc 1,
`data3.cab` on disc 2), and the update packs its own InstallShield setup
into a Microsoft cabinet (MSZIP) inside a self-extractor.
`tools/mkinstall.py` works out, against the images in `rez/isos/` and the
update rez/ carries, where every byte the game needs is, one way per
release (the cabinets lie at other offsets on the USA discs), and the
update's cabinet's offset; it reads every way back and checks it against
the retail file before it writes the list. A script then needs no
InstallShield or ISO 9660 code, only an inflater: it recognises each image
by its size and SHA-256, unpacks the update's cabinet in memory, and writes
each of our files from its retail file, with the retail game DLLs inside
the archives as zeros. On Linux `python3`'s zlib inflates (bash and
coreutils do the rest); on Windows, PowerShell 5.1's .NET
`DeflateStream`. An image is a plain `.iso`, redump's `.bin` (or its
`.cue`) or redump's `.zip`, converted to a temporary `.iso` first.

The public archive is made only if `rez/isos/` has the `.iso` of both
discs of both releases: `make retail` fetches the USA ones too
(`build/retail.sh`'s `retail_discs`). `CJACK_ARCHIVES` says which archives
to write (default `private public`): `make CJACK_ARCHIVES=public` (or any
variant target) writes only the public one, and fails rather than skip it
without the disc images. `IMAGE_BUILD=0` makes the targets use the docker
image as it is instead of building it.

### GitHub Actions

`.github/workflows/public-archives.yml` builds the legacy release game and
all three variants with `CJACK_ARCHIVES=public` on every push to `master`,
every pull request and on demand, each in a job of its own and all four at
the same time (one failing still lets the others finish), runs `make
progress` on the legacy release build (a function recorded as
byte-identical that stops matching fails the run), and keeps the four
public archives as the run's artifacts, each the `.tar.gz` itself rather
than a zip around it. The private archives are never made there. Its steps
are the make targets above, so a run can be reproduced locally:

```sh
git submodule update --init                 # toolchains/, rez/, tools/
make image && make retail                   # (CI: the image from its layer cache, IMAGE_BUILD=0)
export CJACK_ARCHIVES=public
make && make progress && make debug && make modern && make modern-debug
ls out/*-public.tar.gz
```

A first job, "prepare", fetches the redump zips of the European and the
USA discs (2.2 GB from archive.org) only if the cache (keyed on
`rez/isos/SHA256SUMS`, against which `rez/fetch-isos.sh` checks them either
way) does not have them, so the builds never all download them at once,
and it writes the docker image's layer cache, which the builds only read.
The submodules are fetched over HTTPS at depth 1.
