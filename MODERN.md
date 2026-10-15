# The modern build

`CJACK_MODERN` is the project's define for everything that departs from the
retail game: bug fixes (`RETAIL_BUGS.md`), new features, and open-source
middleware in place of RAD's. It is set only in the modern build (`make
modern`, `make modern-debug`; `build/variant.sh`), never in the legacy
build, which stays the retail game as far as it can be rebuilt and is the
only build `make progress` scores. Every change goes behind
`#ifdef CJACK_MODERN` with the retail code kept, unchanged, in the `#else`.

This file is the inventory: what the modern build changes, what it could
change next, and what was looked at and left alone, and why. It follows
nolf2src's `MODERN.md` (NOLF2's modern build) in its rules; the items are
Contract J.A.C.K.'s own.

## Rules for this work

* **Reimplement, never copy.** Fan fixes and patches are a fine source for
  *what* is wrong and *why*; their code is not copied here. The same goes
  for nolf2src's NOLF2 fixes: the idea often carries over, since the code is
  the same generation, but each change is made and checked here.
* **The 2003 compiler.** The modern build uses the legacy build's toolchain
  (Visual C++ .NET 2003, the DirectX 8.1 SDK).
* **No SecuROM, no disc check.** The engine built here has no SecuROM; the
  modern build also drops the game's own disc and CD-key checks.

## Planned, from nolf2src's modern build

To bring over once the game runs (each to be checked against Contract
J.A.C.K.'s code first): widescreen menus, HUD and field of view; mouse look
without the retail input's repeated last event; a frame limiter; windowed
modes at any size; our Bink player on FFmpeg in place of RAD's
`binkw32.dll`; Internet play without the dead master servers.

## Done

* **GS-1: GameSpy.dll's handles.** Each request thread's start event no
  longer leaks the event its base class made, and the server browser's
  constructor sets its thread and browser handles before anything can
  fail (`RETAIL_BUGS.md`; src/Game/Libs/GameSpy/WinSync.h,
  GameSpyBrowser.cpp).
* **GAME-1: GetCommandName.** Returns the name it finds instead of always
  its error string (`RETAIL_BUGS.md`;
  src/Game/ClientShellDLL/ProfileMgr.cpp).
* **EN-1: the world shaders' section order.** The sections compare by
  world only among equal base sections, so the sort gets a strict weak
  ordering (`RETAIL_BUGS.md`; src/Engine/runtime/render_a/src/sys/d3d/
  d3d_rendershader_base.h).
* **EN-2: the window keeps its renderer.** In a window the game no longer
  shuts its renderer down and brings it back when it loses the focus (a
  reload of every texture and a black window meanwhile; only a full-screen
  device needs it); it pauses, unless the console variable
  RunInBackground is set, when it plays on behind other windows
  (src/Engine/runtime/kernel/src/sys/win/client.cpp; nolf2src's M-G41).
