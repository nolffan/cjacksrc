#!/usr/bin/env bash
# GameSpy.dll's behaviour against the retail one, under Wine (in the build
# container: `make shell`, or docker run ... bash build/gamespytest.sh).
#
#   offline   the exports exercised without a network: IsCDKeyValid on fixed
#             and 40,000 random keys, the server's property table, challenge
#             and packet handling, the browser's registry and responses.
#             Ours must print exactly what retail prints.
#   lan       a server and a browser over the loopback, in all four pairings
#             of retail's and ours: the browser must find the server and
#             read its keys, as retail and retail do.
#
# Uses out/gamespytest/ for its programs (compiled with VC .NET 2003) and
# out/tools-wine as the Wine prefix.
set -e
source "$(dirname "$0")/env.sh"
cd "$REPO"
mkdir -p out/gamespytest
cp build/gamespytest/*.cpp out/gamespytest/
cd out/gamespytest
for t in offline lan; do
    run_cl /nologo /MD /O2 /GX /I ../../src/Game/Libs/GameSpy $t.cpp wsock32.lib >/dev/null
done
export WINEPREFIX="$REPO/out/tools-wine"
R="$(cjacksrc_wpath "$REPO/out/reference/GameSpy.dll")"
O="$(cjacksrc_wpath "$REPO/src/Game/built/Final/libs/GameSpy/GameSpy.dll")"
rc=0
wine offline.exe "$R" > offline-retail.txt 2>/dev/null
wine offline.exe "$O" > offline-ours.txt 2>/dev/null
if diff -q offline-retail.txt offline-ours.txt >/dev/null; then
    echo "offline: the same ($(grep -c . offline-ours.txt) lines)"
else
    echo "offline: DIFFERENT"; diff offline-retail.txt offline-ours.txt | head -20; rc=1
fi
for pair in "$R $R" "$R $O" "$O $R" "$O $O"; do
    set -- $pair
    s=retail; [ "$1" = "$O" ] && s=ours
    b=retail; [ "$2" = "$O" ] && b=ours
    out="$(timeout 60 wine lan.exe "$1" "$2" 2>/dev/null | grep -E "^found|^details status" | tr '\n' ' ')"
    echo "lan: server $s, browser $b: $out"
    case "$out" in *"found: 'LAN Test' ver '1.1' players '3'"*) ;; *) rc=1 ;; esac
done
exit $rc
