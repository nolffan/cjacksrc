"""What the scoreboard scores: Contract J.A.C.K.'s binaries, and where each one's
own code, and ours, is.

tools/progress.py and tools/ratchet.py read this file and nothing else
about the project, so they know no binary by name: a binary is added here,
not in the tools. Plain data, with the evidence for every address beside
it (the CSVs in this directory are the rest of the evidence).

TARGETS, one dict per shipped binary, in the README's order:

  label      the binary's name, as the README and functions.csv show it
  what       what it is, for the README
  kind       "image": scored function by function against the shipped
             image's code; "resources": a resource-only image, scored
             resource by resource and left out of the total
  shipped    the retail binary, extracted by build/reference.sh
  stem       the prefix of its files here (<stem>-screen-exclude.csv, ...)
             and in out/reference/ (<stem>-functions.csv)
  built      our build of it: a path or glob under the repository, or None
             while it has no source. Its link is read from the response
             file beside it (<built>.rsp, and <built>.cmd for the
             directory the link ran in): every object and library of ours
             it names is scored, and its .map places our functions. A
             target without a build scores as all unwritten
  optional   True: a missing build is expected (no source yet, or another
             project's output still to come), not an error
  app        [(lo, hi)]: the image's own code -- everything the scoreboard
             counts. Above it is the C runtime (the static parts of
             msvcrt.lib, the import thunks) and the exception-handling
             funclets of .text$x: status `crt`, listed, not counted. A
             range after the first is code of the image's own that the
             linker placed above the runtime
  middleware [(lo, hi)]: third-party code linked inside `app`: status
             `middleware`, listed, not counted
  encrypted  [(lo, hi)]: code the retail image keeps encrypted (SecuROM):
             status `encrypted`, listed, not counted -- nothing can be
             compared there
  third_party  link inputs of ours (file names) that build `middleware`:
             left out of our side, so they neither match nor count as
             unplaced
  cites      True when src/ cites this image's addresses in comments: a
             cited address of an unmatched function is `attempted`
  cited_src  the source trees whose comments cite it (default CITED_SRC):
             per target, because the DLLs share the 0x10000000 range
  purecall   the shipped address of the CRT's `_purecall` (every abstract
             vtable's filler), where tools/vtabcheck.py cannot find its
             `jmp [IAT]` thunk itself

The reconstruction tools (tools/tgt.py) read the same entries: a tool is
given a target by label or stem (`Lithtech`, `server`), and its image,
build, link inputs and regions come from here.

Optional per target, for what NOLF's scoreboard needs and this one does
not yet: icl (Intel C/C++ padding and /Qip names), linked_oracle (compare
relocation-free functions against our linked image too), lib_range,
lib_block, own_names / own_symbols (names that say a function's shipped
address) -- see tools/progress.py.

How the ranges were found: `tools/libscan.py <image> <lib>...` puts each
library's functions in the image (here msvc-7.10-3077's msvcrt.lib,
msvcprt.lib, mfcs71.lib, libcmt.lib and libcpmt.lib, and the DirectX 8.1
SDK's d3dx8.lib). The image's own code ends where the runtime's first
member starts: an import-thunk run (`jmp [IAT]`, 6 bytes each) or
msvcprt's `operator new` (newop_s.obj), then msvcrt.lib's own objects.
Until a binary has a link of ours, its boundary is that first hit: marked
"provisional" below, to be pinned from our .map (which names the library
of every function) once there is one, as nolf2src did. All of Contract
J.A.C.K. links the C runtime as a DLL (MSVCR71.dll, with MSVCP71 and
MFC71 where used), except ClientFx.fxd and the resource DLLs, which link
libcmt.lib statically.
"""

REFERENCE = "out/reference"     # build/reference.sh writes it
OUT = "out/progress"            # each run's functions.csv and summary.json
DATA = "progress"               # the recorded baseline and the evidence

# Every hex address cited in these source trees marks the address
# `attempted` in a target with `cites` (an address a reconstruction has
# been written for), unless the target names its own `cited_src`.
CITED_SRC = []

# No prebuilt static library ships with Contract J.A.C.K.: every library is
# built from source and scored inside the binaries that link it.
LIBS = []
SHIPPED_LIB = None
SHIPPED_AS = {}
SHIPPED_ENV = None

DRIVER_OBJS = set()

TARGETS = [
    # ---- the engine ------------------------------------------------------
    dict(label="Lithtech.exe", what="the engine", kind="image",
         shipped=REFERENCE + "/Lithtech.exe", stem="Lithtech",
         built="src/Engine/built/Final/Lithtech/Lithtech.exe", optional=True,
         cites=True, cited_src=["src/Engine/runtime"],
         # SecuROM-wrapped, as NOLF2's was: .rjes and .obfzv sections, no
         # `jmp [IAT]` thunk outside the encrypted pages (every import call
         # redirected: progress/engine/Lithtech-iat-calls.csv), and .text
         # 0x536000..0x53b000 encrypted on disc (five 4 KB pages of 7.1 to
         # 7.7 bits of entropy a byte, against 5 to 5.5 for the code around
         # them). Pinned from our .map (src/Engine/built/Final/Lithtech/
         # Lithtech.map), which links the same pieces in the same order:
         # the engine's own objects, then its libraries (ltmem, StdLith,
         # RezMgr, the renderer, the sound loader, the UI), up to D3DX
         # (d3dxmath.obj's WithinEpsilon at 0x51de34; libscan.py finds 340
         # of d3dx8.lib's functions in one run 0x51de34..0x538527, the
         # encrypted pages inside it: no engine code is encrypted). Then
         # msvcprt's operator new[] thunk (0x538414), msvcrt.lib's static
         # members (__chkstk 0x538420 .. __SEH_prolog) and the import
         # thunks, the .text$x funclets, and from 0x539a80 the dynamic
         # initializers and atexit destructors of every object (the lowest
         # entry of the __xc_a table at .data 0x568190), to 0x54987e:
         # engine code again, the second range. The single-player demo's
         # unprotected engine (2003-10-10, the same Rich header) stands in
         # for the encrypted pages (tools/demoimports.py).
         app=[(0x00401000, 0x0051de34), (0x00539a80, 0x0054987e)],
         encrypted=[(0x00536000, 0x0053b000)],
         # the CRT's `jmp [_purecall]` thunk is in the encrypted pages (the
         # demo's, at retail's address)
         purecall=0x00538534,
         # D3DX, statically linked from the DirectX 8.1 SDK's d3dx8.lib
         # (cl 13.00.9178: the Rich header's 68 objects of tool 0x1d), as in
         # NOLF2's engine, after the engine's libraries. A few of its inline
         # vector operators compiled into the engine's objects are the
         # engine's.
         third_party=["d3dx8.lib"]),
    dict(label="server.dll", what="the engine's server", kind="image",
         shipped=REFERENCE + "/server.dll", stem="server",
         built="src/Engine/built/Final/server/server.dll", optional=True,
         cites=True, cited_src=["src/Engine/runtime"],
         # pinned from our .map: the own objects, ltmem, RezMgr and StdLith,
         # then msvcprt's import thunk (0x10064024) and msvcrt.lib from
         # __ftol2 (0x10064030), the .text$x funclets, and from 0x10064900
         # the dynamic initializers and atexit destructors (the lowest
         # entry of the __xc_a table, .data 0x10073000) to 0x10069f9b
         app=[(0x10001000, 0x10064024), (0x10064900, 0x10069f9b)]),
    dict(label="SndDrv.dll", what="the sound driver", kind="image",
         shipped=REFERENCE + "/SndDrv.dll", stem="SndDrv",
         built="src/Engine/built/Final/snddrv/SndDrv.dll", optional=True,
         # src/Engine/snddrv/snddrv.dsw. As in our map: counter.obj's,
         # parsewavefile.obj's and s_dx8.obj's code, then ltmem.lib's
         # ltmem.obj, linked first (LTMemInit, LTMemTerm, LTMemAlloc,
         # LTMemFree, LTMemReAlloc, the initializer's constructor and
         # destructor, 0x100085d0..0x10008632), then the 14
         # MSACM32 import thunks (the driver calls msacm.h's functions
         # without dllimport), a `jmp` to the next instruction (0x10008686),
         # msvcprt's operator new (0x1000868b) and msvcrt.lib from __aulldiv
         # (0x100086b0)
         app=[(0x10001000, 0x10008632)]),
    dict(label="GameSpy.dll", what="the GameSpy services", kind="image",
         shipped=REFERENCE + "/GameSpy.dll", stem="GameSpy",
         # src/Game/Libs/GameSpy/GameSpy.dsw: Monolith's Game/Libs/GameSpy
         # (C:\proj\to2\source\Game\Libs\GameSpy\Ping.cpp in the image's
         # strings), GameSpy's C SDK (GameSpySDK/: the Rich header's 29 C
         # objects with the runtime's) and the part of WONAPI.lib its CD key
         # check links (src/Game/Libs/WONAPI)
         built="src/Game/built/Final/libs/GameSpy/GameSpy.dll",
         cites=True, cited_src=["src/Game/Libs/GameSpy"],
         # As in our map: Monolith's objects (GameSpyBrowser, GameSpyCommon,
         # GameSpyPatchInfo, GameSpyServer, Ping), then the SDK's objects
         # from darray.obj's ArrayGrow (0x10006390) to sb_serverlist.obj,
         # then WONAPI.lib (CDKey.obj from 0x10014740, CRC.obj,
         # ByteBuffer.obj), then msvcprt's operator new (0x10014e5e) and
         # msvcrt.lib
         app=[(0x10001000, 0x10014e5e)],
         # GameSpy's SDK and WONAPI: third-party, listed, not counted, and
         # our builds of them are left out of our side (third_party)
         middleware=[(0x10006390, 0x10014e5e)],
         third_party=["darray.obj", "gcdkeyc.obj", "gcdkeys.obj",
                      "ghttpBuffer.obj", "ghttpCallbacks.obj", "ghttpCommon.obj",
                      "ghttpConnection.obj", "ghttpMain.obj", "ghttpPost.obj",
                      "ghttpProcess.obj", "hashtable.obj", "md5c.obj",
                      "natneg.obj", "nonport.obj", "ptMain.obj", "qr2.obj",
                      "qr2regkeys.obj", "sb_crypt.obj", "sb_queryengine.obj",
                      "sb_server.obj", "sb_serverbrowsing.obj",
                      "sb_serverlist.obj", "WONAPI.lib"]),
    dict(label="ltmsg.dll", what="the engine's messages", kind="resources",
         shipped=REFERENCE + "/ltmsg.dll", stem="ltmsg",
         built="src/Engine/built/Final/ltmsg/ltmsg.dll", optional=True),
    # ---- the launcher and the dedicated server ------------------------
    dict(label="ContractJack.exe", what="the launcher", kind="image",
         shipped=REFERENCE + "/ContractJack.exe", stem="ContractJack",
         built="src/Game/built/Final/Launcher/ContractJack.exe", optional=True,
         # provisional: mfcs71.lib's CComCriticalSection::Init at 0x40bd86,
         # then msvcrt.lib
         app=[(0x00401000, 0x0040bd86)]),
    dict(label="ContractJackServer.exe", what="the dedicated server", kind="image",
         shipped=REFERENCE + "/ContractJackServer.exe", stem="ContractJackServer",
         built="src/Game/built/Final/ServerApp/ContractJackServer.exe", optional=True,
         # provisional: mfcs71.lib from 0x414950, then msvcrt.lib from
         # __ftol2 (0x4149a0)
         app=[(0x00401000, 0x00414950)]),
    # ---- the game ------------------------------------------------------
    dict(label="CShell.dll", what="the game: client", kind="image",
         shipped=REFERENCE + "/CShell.dll", stem="CShell",
         built="src/Game/built/Final/ClientShellDLL/TO2/CShell.dll", optional=True,
         # provisional: msvcprt's operator new at 0x10126185, msvcrt.lib from
         # 0x101261fe
         app=[(0x10001000, 0x10126185)]),
    dict(label="Object.lto", what="the game: server objects", kind="image",
         shipped=REFERENCE + "/Object.lto", stem="Object",
         built="src/Game/built/Final/ObjectDLL/TO2/Object.lto", optional=True,
         # provisional: an import thunk at 0x10174d35, msvcprt's operator new
         # (0x10174d3a), msvcrt.lib from 0x10174dae
         app=[(0x10001000, 0x10174d35)]),
    dict(label="ClientFx.fxd", what="the game: client effects", kind="image",
         shipped=REFERENCE + "/ClientFx.fxd", stem="ClientFx",
         built="src/Game/built/Final/ClientFxDLL/ClientFx.fxd", optional=True,
         # links libcmt.lib statically; provisional: a 5-byte jmp at
         # 0x1001b46b, then libcpmt.lib's operator new (0x1001b470) and
         # libcmt.lib from 0x1001b5f3
         app=[(0x10001000, 0x1001b46b)]),
    dict(label="CRes.dll", what="the game's client resources",
         kind="resources", shipped=REFERENCE + "/CRes.dll", stem="CRes",
         built="src/Game/built/Final/ClientRes/TO2/CRes.dll", optional=True),
    dict(label="SRes.dll", what="the game's server resources",
         kind="resources", shipped=REFERENCE + "/SRes.dll", stem="SRes",
         built="src/Game/built/Final/ServerRes/TO2/SRes.dll", optional=True),
]
