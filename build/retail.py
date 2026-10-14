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
