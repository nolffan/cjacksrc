#!/usr/bin/env bash
# Extract the retail binaries -- the engine (Lithtech.exe, server.dll,
# SndDrv.dll, ltmsg.dll, GameSpy.dll), the launcher ContractJack.exe, the
# dedicated server ContractJackServer.exe, and the 1.1 game DLLs (CShell.dll,
# Object.lto, CRes.dll out of Update_v1x1.rez; ClientFx.fxd, SRes.dll out of
# GameDLL.rez: build/retail.py) -- from the installed retail game
# (build/retail.sh) into out/reference/, with their function and vtable
# tables (<stem>-functions.csv, <stem>-vtables.csv), for the checks that
# compare our build against them. Nothing in the build reads them, and
# nothing in out/reference/ is ever run.
#
#   build/reference.sh            (in the build container, or on a host with
#                                  Python 3; no Wine involved)
set -e
cd "$(dirname "${BASH_SOURCE[0]}")/.."
source build/retail.sh
python3 tools/retailextract.py reference "$(retail_dir)" out/reference

# The function table of each image (tools/enumfuncs.py --csv): addr, size,
# how the start was found, and the name an error-tag literal or export gives
# it. The reconstruction tools read these (fndiff.py's sizes, tagmap.py,
# uncovered.py, ...); like the images, they are derived, never committed.
for pair in server:server.dll Lithtech:Lithtech.exe SndDrv:SndDrv.dll GameSpy:GameSpy.dll \
            ContractJack:ContractJack.exe ContractJackServer:ContractJackServer.exe \
            CShell:CShell.dll Object:Object.lto ClientFx:ClientFx.fxd; do
    python3 tools/enumfuncs.py "out/reference/${pair#*:}" \
        --csv "out/reference/${pair%%:*}-functions.csv" | tail -1
done
# ... and its vtables, one row per slot (tools/vtabaudit.py), which
# tools/vtabcheck.py compares our linked images against.
for pair in server:server.dll Lithtech:Lithtech.exe; do
    python3 tools/vtabaudit.py "out/reference/${pair#*:}" \
        --csv="out/reference/${pair%%:*}-vtables.csv" | tail -1
done
