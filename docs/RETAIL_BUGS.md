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

**Fixed so far** (in the modern build): the two GameSpy handle
bugs (MODERN.md GS-1), GetCommandName (GAME-1), the chassis's update
(GAME-2), the join screen's deadlock (GAME-3), a body's spears
(GAME-4), the message queue's lost message (GAME-5), the world shaders' section order (EN-1), ShowVersionInfo (EN-3),
the music thread's handle (EN-4), a failed transition segment (EN-5),
a music intensity out of range (EN-60), the shadows' culling buffer
thrown away per section (EN-61),
batched screen quads' colours (EN-6), SoundSys's null Term (EN-7),
SetCamera's unset viewport (EN-8), the launcher's path overrun (APPS-1),
find handles (APPS-2), "Contract J.A.C.K. 2" (APPS-4) and the registry
opened for writing (APPS-7), the dedicated server's console formats
(APPS-3), a gamepad's buttons bound as axes (CL-24), the end-of-mission
screen's missing record (CL-50), the scope's and camera's masks short
of the view's sides (CL-51), and six that come
with high frame rates: the AIs slowed (EN-41), the player's run, crouch
and jump (CL-41), the float and jump out of water (CL-42), particle and
line systems gone dark (CL-44), aim, sparks and debris (CL-45) and the
music's moods (GAME-40). Twenty-nine in all.

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

### A chassis's update to the clients does not say what it is for

Object.lto 0x100aca0f (Chassis::CreateSFXMessage,
src/Game/ObjectDLL/Chassis.cpp). The chassis sends the clients a
MID_SFX_MESSAGE with its team, drop-zone radius and activate type, but
without the SFX id (SFX_CHASSIS_ID) and the object that every such message
starts with, so a client takes the team for the SFX id and part of the
radius for the object, finds no special fx and drops the message. The
special-fx message the chassis is created with is right; only the update
is lost. It is sent when the chassis is created or loaded, before any
client could use it, so nothing shows in the game.

Fixed in the modern build: src/Game/ObjectDLL/Chassis.cpp, MODERN.md
GAME-2.

### The join screen can deadlock with the server browsers' thread

CShell.dll 0x100cb500 (CScreenJoin::OnServerInfo) and 0x100cc230
(CScreenJoin::Render), src/Game/ClientShellDLL/ScreenJoin.cpp. A browser
reports each server it finds from its own thread, inside its callback,
holding its own locks; OnServerInfo files the server and then takes the
screen's lock to add the server's row to the list. The screen's render
holds that same lock while its update asks the browsers for their status
and halts their requests, which takes the browsers' locks. If a server
arrives while the screen is rendering a search step, each thread waits for
the other's lock and the game hangs on the join screen. OnServerInfo also
changes the server list without the screen's lock while the render can be
reading it.

Fixed in the modern build: src/Game/ClientShellDLL/ScreenJoin.cpp,
MODERN.md GAME-3.

### A body's spears are removed with the wrong object's touch notify cleared

Object.lto 0x10093180 (Body::RemovePowerupObjects,
src/Game/ObjectDLL/Body.cpp). Before it removes each weapon and spear
attached to a body, the function clears the object's FLAG_TOUCH_NOTIFY.
The spears' loop clears it on `m_ahWeapons[iWeapon]` instead of the
spear: the weapons' loop counter, still in scope after its loop, is
kMaxWeapons, so it reads the handle one past the end of m_ahWeapons
(whatever member follows the array, at 0x8dc) and changes that object's
flags, or none. The spears keep their touch notify until they're
removed, so nothing shows in the game.

Fixed in the modern build: src/Game/ObjectDLL/Body.cpp, MODERN.md GAME-4.

### A message queue with no active messages writes through NULL

CShell.dll 0x1005a820 (CHUDMessageQueue::AddMessage,
src/Game/ClientShellDLL/HUDMessageQueue.cpp). To show a message the queue
reuses its oldest active message; when it has none it allocates a new
CHUDMessage but drops the pointer, then creates and shows the message
through NULL. Init() fills the list with MaxMessages messages and leaves it
empty only when every one fails to create, so the game should never reach
it; when it does, the first chat or pickup message crashes the client and
leaks the allocation.

Fixed in the modern build: src/Game/ClientShellDLL/HUDMessageQueue.cpp,
MODERN.md GAME-5.

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

### An intensity outside the music's range reads past its transitions

Lithtech.exe 0x439630 (CLTDirectMusicMgr::GetTransition, src/Engine/
runtime/kernel/src/sys/win/ltdirectmusic_impl.cpp). The transition table
has (I+1)*(I+1)+1 entries for I intensities, indexed by
from+1+to*(I+1). Retail checks only that each intensity is between 1 and
the table's size, not the index itself, so an intensity number above I
(a mission asking for an intensity its control file does not define, or
a control file's transition naming one) reads, and when the control file
is loaded writes, past the end of the table. The 2006 tree checks the
index as well.

Fixed in the modern build: src/Engine/runtime/kernel/src/sys/win/
ltdirectmusic_impl.cpp, MODERN.md EN-60.

### The scope's and camera's masks fall short of the view's sides

CShell.dll's scope and camera overlays (the masks `Interface\hud\Spr\
ssmask.spr` and the scope's, drawn by the interface manager). Their black
stops a strip about a fortieth of the screen's height short of each side
of the 4:3 view, so the world shows down both edges while zoomed: about 12
pixels a side at 640x480. The art is NOLF2's, byte for byte, and so is the
gap (nolf2src's M-G90).

Fixed in the modern build: src/Game/ClientShellDLL/HUDCrosshair.cpp,
MODERN.md CL-51.

### Projected shadows throw their culling buffer away for every section

CModelShadowShader::RenderSection (src/Engine/runtime/render_a/src/sys/
d3d/modelshadowshader.cpp). With Shadows on High
(`ModelShadow_Proj_BackFaceCull 1`) every world section inside a shadow's
projection is culled into a temporary index buffer, locked with
D3DLOCK_DISCARD for every section whether or not a triangle survives;
m_nTempIBIndex is only ever reset, so its NOOVERWRITE branch and wrap
test are dead and every draw reads from index 0. Correct output, but the
buffer is thrown away hundreds of times a second (Wine logs "Filtering out
redundant discard"). As nolf2src's M-E69.

Fixed in the modern build: modelshadowshader.cpp, MODERN.md EN-61.

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

### No sound driver at all crashes the engine

Lithtech.exe 0x49bc80 (SoundSys, src/Engine/runtime/sound/src/
soundmgr.cpp). When the configured sound driver fails and the default
`SndDrv.dll` cannot be made either, SoundSys finds the driver pointer null
and calls `Term` through it (0x49bd0b): a machine without a working sound
driver crashes instead of running silent.

Fixed in the modern build: src/Engine/runtime/sound/src/soundmgr.cpp,
MODERN.md EN-7.

### DrawPrim sets a viewport of undefined size

Lithtech.exe 0x41c1f0 (CD3DDrawPrim::SetCamera, src/Engine/runtime/
render_b/src/sys/d3d/d3ddrawprim.cpp). When the draw-prim camera's
rectangle (or, with no camera, the screen) differs from the current
viewport, SetCamera stores the new size in its saved viewport but builds
the viewport it passes to SetViewport with only the position and depth
range: its Width and Height are whatever the stack held. Direct3D either
refuses the call or clips draw-prim output to a nonsense rectangle.

Fixed in the modern build: src/Engine/runtime/render_b/src/sys/d3d/
d3ddrawprim.cpp, MODERN.md EN-8.

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

### The launcher calls the game "Contract J.A.C.K. 2"

ContractJack.exe, string table (IDS_CANTFINDREZFILE, 25;
src/Game/Launcher/TO2/RES/Lang/EN/Launcher.rc). The message for a missing
Game.rez reads "Please make sure that Contract J.A.C.K. 2 is installed
properly": NOLF2's "No One Lives Forever 2" replaced word for word. It also
sends the player to the disc, which the game never reads (MODERN.md
APPS-4).

Fixed in the modern build: src/Game/Launcher/TO2/RES/Lang/EN/Launcher.rc,
MODERN.md APPS-4.

### The launcher and the game open the machine's registry key for writing just to read it

ContractJack.exe 0x406080 (CLauncherApp::OpenRegistryKey,
src/Game/Launcher/Launcher.cpp, through CRegMgr32::OpenKey), CShell.dll
0x1010d040 and Object.lto 0x1015cce0 (CVersionMgr::CVersionMgr,
src/Game/Shared/VersionMgr.cpp, through CRegMgr::Init). Both open
`HKEY_LOCAL_MACHINE\SOFTWARE\Monolith Productions\Contract Jack\1.0`
for reading and writing -- the game creates every key on the way -- even
where they only read it, and the launcher decides from that open alone
whether the game is installed. Only an administrator may write there: a
user who is not one is served by UAC's registry virtualization (the
programs have no manifest) or not at all, when the launcher finds no
installation and the game keeps nothing (the profile last used). NOLF2's
launcher and game have the same code.

Fixed in the modern build: src/Game/Launcher/Launcher.cpp,
src/Game/Shared/VersionMgr.cpp, MODERN.md APPS-7.

### A gamepad's buttons are bound to its axes

CShell.dll 0x100bc3d0 (CScreenConfigure::Bind,
src/Game/ClientShellDLL/ScreenConfigure.cpp). Configure controls names a
key by its scan code (`##57`) and means to name a joystick's or a
gamepad's control by its name, but its test, `deviceType !=
DEVICETYPE_JOYSTICK || deviceType == DEVICETYPE_GAMEPAD`, is true for a
gamepad, so a gamepad's control is bound as `##<instance number>`. The
engine (input_AddTrigger, Lithtech.exe's input.cpp) takes `##N` for the
first of the device's objects whose DirectInput instance number is N, and
a gamepad's axes come before its buttons: button 1 and up are bound to an
axis. NOLF2's Bind has the same test.

Fixed in the modern build: src/Game/ClientShellDLL/ScreenConfigure.cpp,
MODERN.md CL-24.

### Above 100 frames a second the AIs slow down

server.dll 0x1003f620 (CServerMgr::Update,
src/Engine/runtime/server/src/servermgr.cpp). In single player the client
runs the server once a frame, and each tick advances the game by the time
since the last, in whole milliseconds. An AI asks to be updated every 10
ms (`c_fUpdateDelta`, src/Game/ObjectDLL/AIUtils.cpp) and moves, turns
and counts down by the server's frame time: the time since the last
*tick*, not since its own last update. Up to 100 frames a second each
tick covers at least 10 ms and the AI is updated every tick; above that it
is updated on one tick in two, three or more but steps by one tick's time.
Measured (a temporary log of an AI's updates, M01S01): its frame times
added up to 1.00 of the game time at 60 frames a second, 0.33 at 299 and
0.11 at 1160 -- a third and a ninth of its speed. Any other object updated
less often than every frame does the same. The game time itself runs at
real speed at any rate. The retail game never limits its frame rate in
the final build (`MaxFPS`: MODERN.md EN-40).

Fixed in the modern build: src/Engine/runtime/server/src/servermgr.cpp,
MODERN.md EN-41 (a tick at most every 10 ms).

### The music's moods drop out above 100 frames a second

Object.lto 0x100ff850 (CMusicMgr::Update), 0x100ff370 (CMusicMgr::DoMood,
src/Game/ObjectDLL/MusicMgr.cpp) and 0x10072200 (CAIState::UpdateMusic,
AIState.cpp). Every server frame takes one frame's time off every mood;
an AI puts one frame's time back on the mood it wants, but from its own
update, at most every 10 ms. With server frames shorter than that (above
100 frames a second, the bug above) the moods drain faster than they are
kept up, and the warning and combat music drop out. nolf2src's M-G30
(NOLF2's code is the same).

Fixed in the modern build: src/Game/ObjectDLL/MusicMgr.cpp, AIState.cpp,
MODERN.md GAME-40 (and EN-41 keeps the server's frames at 10 ms or more).

### The player runs and crouches slower, and jumps higher, at high frame rates

CShell.dll 0x1002e4c0 (CMoveMgr::UpdateNormalMotion), 0x1002c8c0
(MoveLocalSolidObject), 0x1002eea0 (UpdateMotion), 0x1002f000
(UpdatePushers; src/Game/ClientShellDLL/CMoveMgr.cpp). The player's speed
is cut back to the cap at the start of each frame and the frame's push
then takes it past the cap until the next: the shorter the frame, the
slower the run. A frame's move shorter than 0.1 units is dropped, which is
6 units a second at 60 frames a second but 100 at 1000, near a crouching
walk's speed. The "can't move" damping halves the speed every frame, the
spectator's takes 0.9 of it every frame, and a pusher (an explosion's, a
vehicle's impact) adds its whole push every frame it lasts. And a jump
peaks about half a frame's fall below v*v/2g, since the engine takes each
frame's gravity off the speed before moving by it: the faster the frame
rate, the higher. Measured (a temporary per-frame log, M01S01): running
197.0 units a second at 60 frames a second, 184.1 at 299, 181.6 at 1160;
crouching 118.0, 111.8, 111.4; a standing jump 68.9 units high at 60,
74.0 at 299, 72.4 at 1160. (CVehicleMgr::UpdateVehicleMotion caps a
vehicle's speed the same way; it is not yet byte-identical, so it is not
listed.)

Fixed in the modern build: src/Game/ClientShellDLL/CMoveMgr.cpp, MODERN.md
CL-41 (and the vehicles, CL-43).

### Out of the water, the player floats lower and jumps lower at high frame rates

CShell.dll 0x1002c370 (CMoveMgr::UpdateInLiquid) and 0x1002e4c0
(UpdateNormalMotion, src/Game/ClientShellDLL/CMoveMgr.cpp). Swimming up,
the rise stops the frame after the head (the camera, which lags a frame)
breaks the surface, so the faster the frame rate the lower the player
floats; and a jump out of the water also gets that frame's swim-up push,
so its height depends on the frame's length. NOLF1's and NOLF2's bug
(nolfsrc and nolf2src measured it: 3 units lower at 120 frames a second
than at 60 in NOLF2's pool); not measured here.

Fixed in the modern build: src/Game/ClientShellDLL/CMoveMgr.cpp, MODERN.md
CL-42.

### Particle and line systems stop emitting at high frame rates

CShell.dll 0x10093430 (CParticleSystemFX::Update, src/Game/ClientShellDLL/
ParticleSystemFX.cpp) and 0x1007db20 (CLineSystemFX::Update,
LineSystemFX.cpp). Each update adds `floor(rate * dt)` particles (lines)
and restarts the clock, so the fraction is lost every frame: where a frame
is shorter than one particle's worth of time none is ever added (at 100
frames a second, every emitter slower than 100 a second; the weather's
snow and rain are such systems). And a system that thins out with the
camera's distance turns any count, 0 too, into one, so it adds one every
frame.

Fixed in the modern build: ParticleSystemFX.cpp, LineSystemFX.cpp,
MODERN.md CL-44.

### Aim, fire sparks and debris change with the frame rate

CShell.dll 0x10024400 (CClientWeapon::UpdateMovementPerturb,
src/Game/ClientShellDLL/ClientWeapon.cpp), 0x1003a7c0 (CFireFX::Update,
FireFX.cpp) and 0x10019dd0 (BounceMovingObject, client_physics.cpp).
Three amounts are per frame where they mean per time: the weapon's
inaccuracy from turning takes the turn since the last frame (a fifth as
much at 300 frames a second as at 60); a fire makes a spark with one
chance in ten every frame; bouncing debris comes to rest below 15/sqrt(dt)
units a second (116 at 60, 260 at 300).

Fixed in the modern build: ClientWeapon.cpp, FireFX.cpp,
client_physics.cpp, MODERN.md CL-45.

## NOLF2's bugs Contract J.A.C.K. no longer has

(none checked yet)

### The end-of-mission screen reads a mission record it may not have

CShell.dll 0x100c0920 (CScreenEndMission::OnFocus, src/Game/ClientShellDLL/
ScreenEndMission.cpp). The mission's name is shown only when
`g_pMissionButeMgr->GetMission` found a record, but a few lines on the
secrets line reads that record's `nNumSecrets` without testing it (the
load at 0x100c0afc), so a mission with no record (a world played outside
the mission list) reads through NULL there.

Fixed in the modern build: ScreenEndMission.cpp, MODERN.md CL-50.
