#!/usr/bin/env bash
# The control for runtime tests: the retail game's own directory, its game
# DLLs from its rez files included, with our engine (Lithtech.exe,
# server.dll) in place of retail's, whose SecuROM refuses to run here.
# What it does differently from out/cjack-legacy-release is the game code's
# doing (CShell.dll, Object.lto, ClientFx.fxd, CRes.dll, SRes.dll).
#
#   build/control.sh [gamedir] [out]
#
# gamedir: the build whose engine is used (default out/cjack-legacy-release);
# out: the control directory (default out/cjack-control), symlinks into
# rez/games/contract-jack-eu-1.1 (relative, so they resolve in the build
# container too) but for the engine (copied), the command file (no custom/)
# and the shipped autoexec.cfg and Default.prf that build/playtest.sh and
# build/mptest.sh start from.
set -eu
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
GAME="$REPO/${1:-out/cjack-legacy-release}"
OUT="$REPO/${2:-out/cjack-control}"
RETAIL="$REPO/rez/games/contract-jack-eu-1.1"
for f in Lithtech.exe server.dll autoexec.cfg.retail Default.prf.retail; do
    [ -f "$GAME/$f" ] || { echo "control.sh: $GAME/$f missing -- build it first"; exit 2; }
done
[ -f "$RETAIL/GameDLL.rez" ] || { echo "control.sh: no retail game in $RETAIL (make retail)"; exit 2; }
rm -rf "$OUT"; mkdir -p "$OUT"
REL="$(python3 -c 'import os,sys; print(os.path.relpath(sys.argv[1], sys.argv[2]))' "$RETAIL" "$OUT")"
for f in "$RETAIL"/*; do
    b=$(basename "$f")
    case "$b" in Lithtech.exe|server.dll|autoexec.cfg|*.url) continue ;; esac
    ln -s "$REL/$b" "$OUT/$b"
done
cp "$GAME/Lithtech.exe" "$GAME/server.dll" "$GAME/autoexec.cfg.retail" "$GAME/Default.prf.retail" "$OUT/"
printf -- '-rez Game.rez -rez GameDLL.rez -rez Sound.rez -rez Update_v1x1.rez\r\n' > "$OUT/cjackcmds.txt"
echo "control: ${2:-out/cjack-control} (retail's game, our engine from ${1:-out/cjack-legacy-release})"
