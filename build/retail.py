# The retail game's layout, for tools/retailextract.py (it reads this file
# from build/; see its header): Contract J.A.C.K. (Europe) with the English
# 1.1 update, as rez/ installs it (rez/games/contract-jack-eu-1.1,
# build/retail.sh).

# The archives, under their installed names. GameDLL.rez holds the 1.0 game
# DLLs and Update_v1x1.rez the 1.1 ones that replace three of them; our
# copies of both have every PE entry zeroed.
REZ = ["Engine.REZ", "Game.rez", "GameDLL.rez", "Sound.rez", "Update_v1x1.rez"]

DATA_DIRS = ["Movies"]
RETAIL_COPIES = [("autoexec.cfg", "autoexec.cfg.retail"),
                 ("Profiles/Default.prf", "Default.prf.retail")]

# Bink, and the Microsoft C, C++ and MFC 7.1 runtime setup installs.
THIRD_PARTY_NAMES = {"binkw32.dll", "mfc71.dll", "msvcp71.dll", "msvcr71.dll"}

# The engine's binaries, ours to build: GameSpy.dll is Monolith's
# Game/Libs/GameSpy around GameSpy's SDK, which the engine and the game
# DLLs both import.
BUILT_ENGINE = ["Lithtech.exe", "server.dll", "SndDrv.dll", "ltmsg.dll", "GameSpy.dll"]

# The retail images the checks compare against: the engine, the launcher
# and the dedicated server, and the game DLLs 1.1 runs -- CShell, Object and
# CRes from Update_v1x1.rez (last in its rez stack), ClientFx and SRes,
# which the update does not replace, from GameDLL.rez.
REFERENCE = BUILT_ENGINE + ["ContractJack.exe", "ContractJackServer.exe"]
REFERENCE_GAME = [("CSHELL.dll", "CShell.dll", "Update_v1x1.rez"),
                  ("OBJECT.lto", "Object.lto", "Update_v1x1.rez"),
                  ("CRES.dll", "CRes.dll", "Update_v1x1.rez"),
                  ("CLIENTFX.fxd", "ClientFx.fxd", "GameDLL.rez"),
                  ("SRES.dll", "SRes.dll", "GameDLL.rez")]

OPEN_RUNTIME = [
    ("binkw32.dll", "src/modern/binkw32 + ext/ffmpeg: our Bink player"),
]

# --- the public archive's install scripts (tools/mkinstall.py) ------------
# The game's data is copied out of the player's own disc images -- the
# European or the USA release's two discs (their InstallShield 6 cabinets
# span both: data1.cab, data2.cab on disc 1, data3.cab on disc 2) -- and
# the 1.1 update, by build/game-scripts/linux-install.sh and
# windows-install.ps1 with the list mkinstall.py writes.
INSTALL_RELEASE = "contract-jack-eu-1.1"
INSTALL_UPDATE = "patches/contractjack_update_en_10_11.exe"
_CJ = "Contract J.A.C.K."
INSTALL_DISCS = {
    "eu1": f"{_CJ} (Europe) (Disc 1)",
    "eu2": f"{_CJ} (Europe) (Disc 2)",
    "us1": f"{_CJ} (USA) (Disc 1)",
    "us2": f"{_CJ} (USA) (Disc 2)",
}
INSTALL_ALIASES = {}
INSTALL_REGIONS = {"Europe": ("eu1", "eu2"), "USA": ("us1", "us2")}
INSTALL_SETS = [("Europe", ["eu1", "eu2"]), ("USA", ["us1", "us2"])]
INSTALL_TAG = "CJACK"
INSTALL_BAT_HEADER = r"""@echo off
rem windows-install.bat -- install the game data from your Contract J.A.C.K. discs.
rem
rem   Drop the images of both discs of Contract J.A.C.K. (the European or
rem   the USA release) and the English 1.1 update,
rem   contractjack_update_en_10_11.exe, onto this file -- or put them in this
rem   folder or the one above it and double-click it. An image is a plain
rem   .iso, redump's .bin (or its .cue), or redump's .zip download; a .bin or
rem   .zip is first converted to a temporary .iso in this folder (about
rem   0.7 GB a disc, removed at the end).
rem
rem It checks each file's SHA-256, copies the game's data out of them into
rem this folder and checks every file it writes. Nothing is installed or run
rem from them, and nothing else is changed.
setlocal
set "CJACK_SELF=%~f0"
set "CJACK_PS1=%TEMP%\cjack-install-%RANDOM%%RANDOM%.ps1"
powershell -NoProfile -ExecutionPolicy Bypass -Command "$t=[IO.File]::ReadAllText($env:CJACK_SELF); $m='#'+'__CJACK_PS__'; $a=$t.IndexOf($m); $b=$t.IndexOf($m,$a+1); [IO.File]::WriteAllText($env:CJACK_PS1,$t.Substring($a,$b-$a))"
if errorlevel 1 goto failed
powershell -NoProfile -ExecutionPolicy Bypass -File "%CJACK_PS1%" "%~f0" %*
set "CJACK_RC=%errorlevel%"
del "%CJACK_PS1%" 2>nul
pause
exit /b %CJACK_RC%
:failed
echo Could not start PowerShell.
pause
exit /b 1"""
