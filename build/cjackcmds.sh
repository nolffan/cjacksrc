#!/usr/bin/env bash
# The game directory's cjackcmds.txt, as a fresh build writes it:
#
#   build/cjackcmds.sh
#
# The rez stack the engine is started with, as the retail ContractJack.exe
# launcher gives it ("lithtech.exe -cmdfile launchcmds.txt"): the game's
# archives (later ones override earlier ones; the engine opens Engine.REZ
# itself), the 1.1 update's archive (its UpdateCommandLine0) and custom/ --
# our game DLLs -- last, so that nothing overrides them. build/all.sh writes
# it into the game directory.
printf -- '-rez Game.rez -rez GameDLL.rez -rez Sound.rez -rez Update_v1x1.rez -rez custom\r\n'
