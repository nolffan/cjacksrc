# Bugs in the retail game

Bugs in *Contract J.A.C.K.* (version 1.1) as it shipped, found while
rebuilding it. Each is a mistake in the code that a source change would
fix. This is a living list: add a bug when it is found, in the format
below, whether or not it is fixed yet.

The legacy build reproduces every one of them, on purpose: compiling to the
retail machine code means compiling its mistakes too. They are fixed in the
**modern build** (`make modern`, the `CJACK_MODERN` define; `MODERN.md`):
each fix goes behind `#ifdef CJACK_MODERN`, with the retail code kept in the
`#else`, so the legacy build is unchanged and still matches
(`make progress`).

An entry that is fixed says so under its heading, with the file the fix is
in and its `MODERN.md` id. One that is not says why, or which part of the
tree it waits for.

Every binary here is a reconstruction (`docs/PLAN.md`), so an entry is
listed only where our function is byte-identical to the retail one at the
address it names (`tools/exematch.py`; for a string, the string is in the
retail image too): it describes what the retail code does, not a guess.
Addresses are as in the retail images (`build/reference.sh` extracts them
into `out/reference/`).

Many of NOLF2's bugs (nolf2src's `RETAIL_BUGS.md`) are likely to be here
too, since Contract J.A.C.K. was built from the same code: check each
against the retail binary before listing it. Those Contract J.A.C.K. no
longer has go at the end, so nobody looks again.

**Fixed so far** (in the modern build): ten -- the two GameSpy handle
bugs (MODERN.md GS-1), GetCommandName (GAME-1), the world shaders' section
order (EN-1), ShowVersionInfo (EN-3), the music thread's handle (EN-4), a
failed transition segment (EN-5), the launcher's path overrun (APPS-1) and
find handles (APPS-2), and the dedicated server's console formats
(APPS-3).

## Format

```
### <What goes wrong, as a player or a programmer would put it>

<binary> <address> (<function>, <source file>). What the code does, and
what it should do; how it shows in the game, if it does.

Fixed in the modern build: <file>, MODERN.md <id>. (Or: not fixed, because ...)
```

## The bugs

### The sound driver frees two arrays with `delete` instead of `delete[]`

SndDrv.dll 0x10002040 (HasFormat, src/Engine/snddrv/s_dx8.cpp) and
0x10005a71 (CDx8SoundSys::Init3DSampleFromFile, the same file). HasFormat
allocates its WAVEFORMATEX with `new BYTE[dwSize]` and
Init3DSampleFromFile its decompressed sample with `new uint8[...]`, and
both free them with scalar `delete` (operator delete, 0x1000881c) rather
than `delete[]` (0x10008822, which every other such buffer in the driver
gets). Undefined behaviour in C++; harmless with Visual C++'s runtime,
where both operators free the block, so nothing shows in the game.

Not fixed: it changes nothing at run time with the runtime the game ships
with.

### Every GameSpy request thread leaks an event handle

GameSpy.dll 0x100021b0 (CGameSpyBrowser::CGameSpyBrowser), 0x10003550
(CGameSpyPatchInfo::CGameSpyPatchInfo) and 0x10004630
(CGameSpyServer::CGameSpyServer), src/Game/Libs/GameSpy/WinSync.h. Each
object's thread-started event is a CWinSync_PulseEvent, whose constructor
creates an auto-reset event over the handle its base class
CWinSync_Event's constructor has just created (manual-reset): three
CreateEventA calls for two events, and the first handle is never closed.
One handle leaks per browser, patch request and server created -- a few
per game session; nothing shows.

Fixed in the modern build: src/Game/Libs/GameSpy/WinSync.h, MODERN.md
GS-1.

### The server browser's destructor can use uninitialised handles

GameSpy.dll 0x100021b0 (CGameSpyBrowser::CGameSpyBrowser,
src/Game/Libs/GameSpy/GameSpyBrowser.cpp). The constructor sets the
thread handle and the SDK's ServerBrowser only at its end; if the game
type has no GameSpy names (CGameSpyCommon::GetQueryGameName and friends
fail), it returns with both uninitialised, and the destructor (0x100020e0)
then signals, waits on, terminates and closes whatever the thread handle
holds and frees whatever the browser pointer holds. The retail game
always passes type 1, so it never happens.

Fixed in the modern build: src/Game/Libs/GameSpy/GameSpyBrowser.cpp,
MODERN.md GS-1.

### The server's CD key challenge is one byte longer than it says

GameSpy.dll 0x10003a90 (CGameSpyServer::GetCDKeyChallenge,
src/Game/Libs/GameSpy/GameSpyServer.cpp). It reports a length of 8 and
accepts a buffer of 8, then writes 8 letters and a terminating zero: a
caller that sizes its buffer from the reported length gets a byte written
past it.

Not fixed: the right fix depends on how Object.lto sizes the buffer
(package 4, the game, will show it); a buffer of 8 would make a stricter
check refuse every challenge.

### GetCommandName always answers "Error in CommandName()!"

CShell.dll 0x100aa8d0 (GetCommandName,
src/Game/ClientShellDLL/ProfileMgr.cpp). The loop over the command table
copies the matching action's name into its buffer but neither breaks nor
returns, and the error string is then copied over it unconditionally, so
every command gets the error string. Nothing in the retail CShell.dll
calls it, so nothing shows in the game.

Fixed in the modern build: src/Game/ClientShellDLL/ProfileMgr.cpp,
MODERN.md GAME-1.

### Two console variables take a float's value as an integer's bits

Lithtech.exe and server.dll's engine-variable tables (the entries for
`MaxExtrapolateTime` and `MaxExtrapolateDist`, src/Engine/runtime/shared/
src/engine_vars.cpp) and cc_SetEngineVar (server.dll 0x10009490,
src/Engine/runtime/shared/src/concommand.cpp). Both variables are floats
(0.5 seconds, 64 units), but the table registers them with `EV_LONG`, so
the integer slot points at them: setting `MaxExtrapolateTime 1` from the
console stores the integer 1, a float of 1.4e-45, and the client's
prediction (predict.cpp) all but stops extrapolating; `MaxExtrapolateTime
0.25` stores 0. The defaults are right only because nobody sets them.

Not fixed yet: in the modern build the two entries would be `EV_FLOAT`.

### The world's shader sections are sorted with an inconsistent order

Lithtech.exe 0x4c16b6 and the other instances of std::sort's helpers the
world shaders inline it into (CRenderShader_Base::CInternalSection's
operator<, src/Engine/runtime/render_a/src/sys/d3d/
d3d_rendershader_base.h, through SortSections' FSortSections). A section
sorts before another if its base section (texture, flags) is less, or
else if its world is less, and only then are unequal base sections
ordered after: so a section with the greater texture but the lesser world
sorts both before and after the other. std::sort needs a strict weak
ordering; given this one it can order sections arbitrarily, and its
unguarded insertion pass can in principle step outside the range. In
practice a level's world models seldom share a shader with the main
world in a way that triggers it.

Fixed in the modern build: src/Engine/runtime/render_a/src/sys/d3d/
d3d_rendershader_base.h, MODERN.md EN-1.

### The ShowVersionInfo console command calls a null function

Lithtech.exe 0x41a360 (con_ShowVersionInfo, src/Engine/runtime/client/src/
consolecommands.cpp) and 0x40bcd0 (CLTClient::InitFunctionPointers,
clientde_impl.cpp). ILTClient's GetVersionInfoExt and GetPerformanceInfo
are never set (the engine leaves them NULL), yet ShowVersionInfo calls
GetVersionInfoExt without testing it, as NOLF2's did: typing the command
in the console crashes the game.

Fixed in the modern build: src/Engine/runtime/client/src/clientde_impl.cpp
and consolecommands.cpp, MODERN.md EN-3.

### The music manager leaks its notification thread's handle

Lithtech.exe 0x43b2a0 (CLTDirectMusicMgr::Term, src/Engine/runtime/kernel/
src/sys/win/ltdirectmusic_impl.cpp). Term tells the DirectMusic
notification thread to exit and waits for it, but never closes the
thread's handle, which Init makes each time: one handle leaks for each
time the music manager is shut down (NOLF2's closes it). Nothing shows.

Fixed in the modern build: src/Engine/runtime/kernel/src/sys/win/
ltdirectmusic_impl.cpp, MODERN.md EN-4.

### A failed transition segment crashes the music manager

Lithtech.exe 0x43b720 (CLTDirectMusicMgr::ChangeIntensity, src/Engine/
runtime/kernel/src/sys/win/ltdirectmusic_impl.cpp). When an intensity
change plays a transition segment, the segment state PlaySegment returns
is queried for IDirectMusicSegmentState8 without a test: if DirectMusic
fails to play the segment, the state is NULL and the game crashes. NOLF2's
code tests it (a fix marked "[kml]").

Fixed in the modern build: src/Engine/runtime/kernel/src/sys/win/
ltdirectmusic_impl.cpp, MODERN.md EN-5.

### Batched screen-space quads lose their vertex colours

Lithtech.exe 0x41d7e0 (CD3DDrawPrim::DrawPrim for LT_POLYGT4,
src/Engine/runtime/render_b/src/sys/d3d/d3ddrawprim.cpp). When more than
two textured Gouraud quads are drawn in screen space they are copied into
a triangle list, and every one of the six vertices takes the colour of the
quad's first vertex (0x41d8ef): a gradient across a quad drawn this way
comes out flat. Two quads or fewer, or a 3D transform, keep each vertex's
colour.

Fixed in the modern build: src/Engine/runtime/render_b/src/sys/d3d/
d3ddrawprim.cpp, MODERN.md EN-6.

### The launcher's own path can overrun a buffer

ContractJack.exe 0x40669d (CLauncherApp::InitInstance,
src/Game/Launcher/Launcher.cpp). Checking whether the game is already
running, the launcher reads the current directory into `sDir[128]` (at
most 125 characters), appends `\`, and `sprintf`s it with `LITHTECH.EXE`
into `sExe[128]`: a directory of 116 to 124 characters overruns `sExe` on
the stack at start-up. NOLF2's launcher has the same code.

Fixed in the modern build: src/Game/Launcher/Launcher.cpp, MODERN.md
APPS-1.

### The launcher never closes its find handles

ContractJack.exe 0x409210 and 0x409260 (ExistRezFile and ExistRezDir,
src/Game/Launcher/RezFind.cpp; FindRezFiles, 0x4092a0, has both inlined)
and 0x405310 (CDisplayMgr::EnumerateRenderers,
src/Game/Launcher/DisplayMgr.cpp). Each `_findfirst` handle is left open on
every path: the launcher does not even import `_findclose`. Each leaks a
search handle for as long as the launcher runs; harmless in practice, as
NOLF2's launcher showed.

Fixed in the modern build: src/Game/Launcher/RezFind.cpp and
DisplayMgr.cpp, MODERN.md APPS-2.

### The dedicated server's console takes messages as formats

ContractJackServer.exe 0x4097d0 (ScmdConsoleDriver_ServerApp::WriteMessage,
src/Game/ServerApp/ScmdConsoleDriver_ServerApp.cpp) passes each SCMD reply
as the format string of CServerDlg::WriteConsoleString (0x40ac00, a
`_vsnprintf`), and the replies carry text from the players: the client
list (`LISTCLIENTS`) has their names. A player named `%s%s%s` makes the
server read arguments that are not there when its operator lists the
clients, and can crash it. NOLF2's dedicated server has the same code.

Fixed in the modern build: src/Game/ServerApp/
ScmdConsoleDriver_ServerApp.cpp, MODERN.md APPS-3.

## NOLF2's bugs Contract J.A.C.K. no longer has

(none checked yet)
