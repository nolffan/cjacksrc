# Which compiler built what

Every retail PE carries a Rich header, the linker's inventory of the tool
builds that produced its objects (`python3 tools/richhdr.py <image>`). For
Contract J.A.C.K. 1.1 it is the same story in every binary: **Visual C++
.NET 2003, 13.10.3077** (`toolchains/msvc-7.10-3077`). Where a revision or
flag set has been *proven* by byte-matching, this file says so; until then
the attribution is the header's own testimony.

| Binary | Link date | cl 13.10.3077 objects (C / C++) | Also | Built here with |
|---|---|---|---|---|
| `Lithtech.exe` | 2003-10-27 | 11 / 295 | 68 C++ and 3 C objects of cl 13.00.9178 and one of masm 6.14.8444: `d3dx8.lib` of the DirectX 8.1 SDK, as in NOLF2's engine | `/O2 /Ob2`, no `/GX`, `/GS`, `/GR` (below) |
| `server.dll` | 2003-10-27 | 7 / 122 | | `/O2`, no `/GX`, `/GS`, `/GR` (below) |
| `SndDrv.dll` | 2003-10-14 | 6 / 10 | one VC6 C++ object (12.00.8168) and one of cl 13.00.9178: EAX's GUIDs and the DirectX SDK's `dxguid.lib`, as in NOLF2's | -- |
| `ltmsg.dll` | | 7 / 2 | resources and msvcrt's DLL entry | -- |
| `GameSpy.dll` | 2003-10-31 | 29 / 18 | the C objects are GameSpy's SDK (22) and msvcrt.lib's; the C++ ones Monolith's wrapper, WONAPI.lib's CD key check and msvcrt/msvcprt's | `msvc-7.10-3077`: the wrapper `/MD /Ox /GF` (no `/Gy`, no `/GX`, no `NDEBUG`), WONAPI `/MD /O2 /GX /GR` |
| `ContractJack.exe` | 2003-10-31 | 15 / 28 | 9 objects of tool 0x27, build 8491 (not yet identified); MFC 7.1 as a DLL | -- |
| `ContractJackServer.exe` | 2003-10-31 | 15 / 28 | the same 9 of 0x27/8491 | -- |
| `CShell.dll` | 2003-10-31 | 9 / 306 | 3 import descriptors of lib 7.00.9210 (the DirectX 8.1 SDK's `dinput8.lib`) | msvc-7.10-3077, `/MD /O2 /GR`, no `/GX` (LTGUIMgr `/GS`): provisional, below |
| `Object.lto` | 2003-10-31 | 7 / 348 | | msvc-7.10-3077, `/MD /O2 /GR`, no `/GX`: provisional |
| `ClientFx.fxd` | 2003-10-14 | 115 / 41 | `libcmt.lib` statically: most C objects are the CRT's | msvc-7.10-3077, `/MT /O2`, no `/GX`: provisional |
| `CRes.dll`, `SRes.dll` | | 67 / 3 | resources and `libcmt.lib` | msvc-7.10-3077: both retail's but for the time stamp |

The rows at builds 2067 and 2179 (tools 0x5d and 0x69) in every binary come
with Visual C++ .NET 2003's own libraries: a test program built with
`msvc-7.10-3077` against `msvcrt.lib` has the 0x5d ones too
(`toolchains/COMPILERS.md`). The 1.1 update rebuilt only the engine
(`Lithtech.exe`, `server.dll`), the launcher, the dedicated server,
`GameSpy.dll`, `CShell.dll`, `Object.lto` and `CRes.dll`; `SndDrv.dll`,
`ltmsg.dll`, `ClientFx.fxd` and `SRes.dll` are 1.0's.

## Notes

* The engine is built from the LithTech Jupiter source in `../fear` -- the
  2006 distribution of this engine line, itself built with Visual Studio
  .NET 2003 (its `Lithtech.map`) -- so for once the reference tree and the
  retail binaries share a compiler: a near there is a source question, not
  a compiler one, until shown otherwise.
* NOLF2's binaries were VC6 SP5; nothing of nolf2src's VC6-specific codegen
  lore (`guides/09`, `12`) carries over unchecked.
* `GameSpy.dll`: the wrapper has no exception frames at all but for the
  STL's explicit try blocks (so no `/GX`), keeps functions nothing calls
  (no `/Gy`: `/Ox /GF` rather than `/O2`, which implies it -- and cl 13.10
  has no `/Gy-`) and asserts (no `NDEBUG`). Its CD key check is WONAPI's,
  linked from a library compiled with `/GX /GR` and `/Gy` (only the called
  members are in the image). Our Rich header has 12 C++ objects against
  retail's 18: Monolith's project had more files than the five that hold
  code (or WONAPI.lib contributed more members).
* The game DLLs (src/Game/TO2.dsw) start from NOLF2's project flags, with
  what the retail images show: `/GR` everywhere (CShell.dll and Object.lto
  have RTTI for the TO2 project's classes too, `CScreen*`, `CHUD*`, not
  only for the shared libraries' that NOLF2's projects gave `/GR`; both
  import `__RTDynamicCast`); `/GS` only in LTGUIMgr (CShell.dll imports
  `__security_error_handler`, but its only two calls of
  `__security_check_cookie`, 0x10121a92 and 0x10121b3a, are in
  LTGUIMgr's slider; Object.lto has none). `/O2` vs `/Ox` and the other
  code-generation switches are not yet proven: SndDrv.dll turned out to
  be `/Ox /GF`.
* None of the game DLLs, nor the libraries they link (ButeMgr, CryptMgr,
  Lib_Lith, Lib_StdLith, MFCStub, RegMgr, LTGUIMgr), has `/GX`: a
  destructor such as `CLTGUIWindow::~CLTGUIWindow` (CShell 0x1011e560) or
  `CMenuMgr::~CMenuMgr` (0x10084230) destroys its members with no
  exception frame, and the images have few frames at all (CShell.dll 136,
  Object.lto 205, ClientFx.fxd 9, against 855, 1613 and 79 with `/GX`):
  those left are the STL's explicit try blocks. Dropping `/GX` took the
  identical functions from 3,603 to 4,555 in CShell.dll, 5,848 to 7,610 in
  Object.lto and 285 to 393 in ClientFx.fxd.


## The engine's flags

Measured by compiling single files of the engine with each candidate set
(`build/engine.sh`'s loop; the objects matched with `tools/exematch.py`
against retail `server.dll` and the single-player demo's plaintext of
`Lithtech.exe`), and from the images themselves:

| Test | `server.dll` (s_object, servermgr) | `Lithtech.exe` (clientmgr, objectmgr) |
|---|---|---|
| `/O1` | 13, 24 functions identical | 49, 44 |
| `/O2` | **58, 220** | **153, 111** |
| `/O2 /Ob1` | 55, 185 | -- |
| `/O2 /EHsc` | 46, 161 | 118, 98 |
| `/O2 /G7` | 44, 157 | -- |
| `/O2 /GS` | 57, 215 | -- |

- **`/O2`, both.** NOLF2's `server.dll` was `/O1`; Contract J.A.C.K.'s is
  `/O2` (its `.text` is 0x69000 bytes against NOLF2's 0x4d000). `/O2`
  implies `/Ob2` in VC .NET 2003.
- **No `/GX` (`/EHsc`).** The own objects have only the EH frames of the
  standard library's own `try` blocks (48 `mov eax, fs:[0]` in the exe's
  own code, 34 in all of `server.dll`); with `/EHsc` every function with
  a destructible local gets one, and fewer functions match.
- **No `/GS`.** Neither retail image imports `__security_error_handler`
  or has the cookie check, nor does the single-player demo's engine of
  2003-10-10 (Rich header identical to retail's, triple for triple). The
  multiplayer demo's engine and `server.dll` of 2003-10-11 are `/GS`
  builds (they import it, and have three more C objects: the cookie's):
  that demo is a different build, and the less useful stand-in.
- **No `/GR`.** The only RTTI in either image is the library's
  (`exception`, `logic_error`, `length_error`, `out_of_range`,
  `type_info`): VC .NET 2003's own standard library, no STLport.
- The default code generation (`/GB`, blended): `/G6` gives the same, `/G7` loses.
- The libraries too: compiled each way, `d3d_renderworld.cpp` places 988
  of retail's functions without `/GX` and 608 with it, `d3d_draw.cpp` 53
  against 42, the UI's and RezMgr's files likewise: D3DRender, Lib_UI,
  Lib_ILTSound and Lib_RezMgr are `/O2 /Ob2` without `/GX` (nolf2src's
  had it). The renderer's 133 EH frames are the standard library's own
  `try` blocks. Lib_StdLith and Lib_Lith (the game's too) are without
  `/GX` as well, as the game DLLs show them; the engine's ltmem is
  `/Ox /GF` (SndDrv.dll's object of it unchanged).
