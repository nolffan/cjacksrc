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
         # SecuROM's rewritten import calls, put back for scoring
         iat_calls="progress/engine/Lithtech-iat-calls.csv",
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
         # src/Game/TO2.dsw (Launcher). As in our map: the launcher's objects
         # (BitmapCheckButton .. Utils, then the shared VersionMgr.obj from
         # 0x40a820), then regmgr.lib (CRegMgr, from 0x40a9b0) and
         # regmgr32.lib (CRegMgr32, its OpenKey last, to 0x40b8db); from
         # 0x40b8dc the import thunks of the MFC functions called without
         # dllimport (mfc71.lib), mfcs71.lib's CComCriticalSection
         # (0x40bd72), msvcrt.lib, the .text$x funclets (0x40c510). Then
         # the dynamic initializers: theApp's (Launcher.obj, 0x40c530) and
         # the SDK headers' float constants (StdAfx.obj, to 0x40c603), the
         # ATL module's (atls.lib), and the atexit destructors: theApp's
         # (0x40c630), the ATL module's (0x40c63a)
         built="src/Game/built/Final/Launcher/ContractJack.exe", optional=True,
         app=[(0x00401000, 0x0040b8dc), (0x0040c530, 0x0040c603),
              (0x0040c630, 0x0040c63a)]),
    dict(label="ContractJackServer.exe", what="the dedicated server", kind="image",
         shipped=REFERENCE + "/ContractJackServer.exe", stem="ContractJackServer",
         # src/Game/TO2.dsw (ServerApp). As in our map: the server's objects
         # (CommonUtilities .. VersionMgr, the shared ones among them), then
         # its libraries (ButeMgr, CryptMgr, StdLith, RegMgr, RegMgr32, the
         # last CRegMgr32::OpenKey ending at 0x41441b); from 0x41441c the
         # import thunks of the MFC functions called without dllimport,
         # mfcs71.lib (0x41493c), msvcrt.lib, the .text$x funclets
         # (0x415180). Then the dynamic initializers of the server's own
         # objects (the interface holders' and ltmodule's registrations, the
         # SDK headers' float constants, theApp, ButeMgr's statics:
         # 0x4154c0..0x415745) before the ATL module's (atls.lib), and the
         # atexit destructors (0x415770..0x4158c9) before the ATL module's
         built="src/Game/built/Final/ServerApp/ContractJackServer.exe", optional=True,
         app=[(0x00401000, 0x0041441c), (0x004154c0, 0x00415746),
              (0x00415770, 0x004158ca)]),
    # ---- the game ------------------------------------------------------
    dict(label="CShell.dll", what="the game: client", kind="image",
         shipped=REFERENCE + "/CShell.dll", stem="CShell",
         built="src/Game/built/Final/ClientShellDLL/CShell.dll",
         # src/Game/TO2.dsw. As in our map: the game's code (ButeMgr's last)
         # ends where msvcprt's newop_s.obj (operator new, 0x10126185)
         # starts, then newaop_s.obj, the import thunks and msvcrt.lib
         # (from 0x101261fe)
         app=[(0x10001000, 0x10126185)],
         # Functions of ours whose bytes recur at other shipped addresses
         # (each named here is the shipped function at that address, read
         # from the retail code):
         own_symbols={
             # four CSpecialFX::CreateObject bodies in their classes' vtables
             # (CPickupItemFX's is 0x1013497c's third slot, its Init the
             # ClientFX link at +0x40); our link folds three of them into one
             "?CreateObject@CAimMagnetFX@@UAEIPAVILTClient@@@Z": 0x10001ec0,
             "?CreateObject@ChassisFX@@UAEIPAVILTClient@@@Z": 0x10017f20,
             "?CreateObject@ChassisPieceFX@@UAEIPAVILTClient@@@Z": 0x100181a0,
             "?CreateObject@CPickupItemFX@@UAEIPAVILTClient@@@Z": 0x10095e60,
             # SetNormalPhysicsModel, then a jmp to TermLevel: the shape of
             # every such two-call thunk
             "?OnExitWorld@CVehicleMgr@@QAEXXZ": 0x10107850,
             # null test, SetNewLevel, SetLoadingLevel (50 bytes): by size
             # ours took HandleExitLevel's head (0x10088450)
             "?SetupSavedLevel@CMissionMgr@@AAE_NPBD@Z": 0x10088810,
             # InterfaceMgr.obj, where by size each of these took the next
             # one's address: OnCommandOff (the command switch after the
             # chat and menu tests), NextWeapon (the class switch over
             # NEXT_WEAPON to NEXT_WEAPON_6), HandleDisplayTimerMsg (the
             # timer, paused flag and timer id)
             "?OnCommandOff@CInterfaceMgr@@UAEIH@Z": 0x10063ee0,
             "?NextWeapon@CInterfaceMgr@@AAEXH@Z": 0x10065260,
             "?HandleDisplayTimerMsg@CInterfaceMgr@@QAE_NAAVILTMessage_Read@@@Z":
                 0x100653d0,
             # GameClientShell.obj: the effects update without a render,
             # and the switching-worlds state with the asset list's start
             "?UpdateEffectsNoRender@CGameClientShell@@IAEXXZ": 0x10047da0,
             "?HandleMsgSwitchingWorldState@CGameClientShell@@AAEXPAVILTMessage_Read@@@Z":
                 0x10046670,
             # ClientMultiplayerMgr.obj, read from the callers:
             # StartClientServer's tail jumps (type 0, 3, 4), UpdateJoinState's
             # (the NAT states), the three callers of UpdateNetClientData
             # (SetupClient, SetupServerHost, InitMultiPlayer), and the
             # constructor and destructor (vtable 0x1012c0e8)
             "??1ClientMultiplayerMgr@@QAE@XZ": 0x10020150,
             "??0ClientMultiplayerMgr@@QAE@XZ": 0x100201f0,
             "?StartClient@ClientMultiplayerMgr@@IAE_NXZ": 0x1001ed10,
             "?StartServerAsSinglePlayer@ClientMultiplayerMgr@@IAE_NXZ":
                 0x1001ee70,
             "?StartServerAsHost@ClientMultiplayerMgr@@IAE_NXZ": 0x1001ef60,
             "?UpdateNetClientData@ClientMultiplayerMgr@@QAE_NXZ": 0x1001f070,
             "?UpdateNatNegotiation@ClientMultiplayerMgr@@IAEXXZ": 0x1001f190,
             "?UpdateNatWait@ClientMultiplayerMgr@@IAEXXZ": 0x1001f260,
             # the screens' constructors, by their vtables (0x1013664c,
             # 0x10136dec)
             "??0CScreenEndMission@@QAE@XZ": 0x100c0570,
             "??0CScreenHostWeapons@@QAE@XZ": 0x100c7e30,
             # Remove All enabled by the selected list (0x794, 0x79c)
             "?UpdateButtons@CScreenHostWeapons@@IAEXXZ": 0x100c8310,
             # ClearServers, then a jmp to CBaseScreen::Term (vtable
             # 0x10136fc0's entry): the two-call thunk's shape again
             "?Term@CScreenJoin@@UAEXXZ": 0x100cb6d0,
         }),
    dict(label="Object.lto", what="the game: server objects", kind="image",
         shipped=REFERENCE + "/Object.lto", stem="Object",
         built="src/Game/built/Final/ObjectDLL/Object.lto",
         # src/Game/TO2.dsw. As in our map: the game's code (RegMgr's
         # CRegMgr::Init last) ends where msvcprt's runtime starts: here
         # newaop_s.obj first (operator new[], a 5-byte jmp at 0x10174d35),
         # then newop_s.obj (0x10174d3a) and msvcrt.lib from 0x10174dae
         app=[(0x10001000, 0x10174d35)],
         # Functions of ours that the placement by object order puts at a
         # neighbour of a similar size (each named here is the shipped
         # function at that address, read from the retail code):
         own_symbols={
             # retail inlines the set iterator's ++ (136 bytes), ours calls
             # _Inc (88): by size ours took UpdateSpecialFX's 0x10107930
             "?SaveOverlays@CPlayerObj@@QAEXPAVILTMessage_Write@@@Z":
                 0x10110900,
             # the constructor (vtables, CButeMgr::Init, the hash map at
             # 0x11c0); by size ours took 0x100bf8c0, DebugLineSystem's
             # hash_map erase
             "??0CSoundButeMgr@@QAE@XZ": 0x1014a880,
         }),
    dict(label="ClientFx.fxd", what="the game: client effects", kind="image",
         shipped=REFERENCE + "/ClientFx.fxd", stem="ClientFx",
         built="src/Game/built/Final/ClientFxDLL/ClientFx.fxd",
         # links libcmt.lib statically. src/Game/TO2.dsw; as in our map:
         # the effects' code (spritesystemfx.obj last in ours) ends where libcpmt.lib
         # starts, with newaop.obj (operator new[], a 5-byte jmp at
         # 0x1001b46b), newop.obj (0x1001b470), and libcmt.lib from
         # 0x1001b5f3
         app=[(0x10001000, 0x1001b46b)]),
    dict(label="CRes.dll", what="the game's client resources",
         kind="resources", shipped=REFERENCE + "/CRes.dll", stem="CRes",
         built="src/Game/built/Final/ClientRes/TO2/CRes.dll"),
    dict(label="SRes.dll", what="the game's server resources",
         kind="resources", shipped=REFERENCE + "/SRes.dll", stem="SRes",
         built="src/Game/built/Final/ServerRes/TO2/SRes.dll"),
]
