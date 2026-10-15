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

**Fixed so far** (in the modern build): none yet.

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

## NOLF2's bugs Contract J.A.C.K. no longer has

(none checked yet)
