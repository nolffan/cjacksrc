#!/usr/bin/env bash
# The runtime test, for Contract J.A.C.K.: tools/playtest.sh (shared with
# nolf2src, whose game names it assumes) run on a Contract J.A.C.K. game
# directory.
#
#   build/playtest.sh [gamedir] [world] [seconds]     (in the build container;
#                                                       `make test` runs it)
#
# Everything is tools/playtest.sh's (its header documents the arguments, the
# PLAYTEST_* settings, the input scripts and the results in out/play/<name>/),
# with Contract J.A.C.K.'s names in place of NOLF2's: the command file
# cjackcmds.txt (build/cjackcmds.sh), the game directory
# out/cjack-legacy-release, the first single-player world
# Worlds\RetailSinglePlayer\M01S01. The adapted copy is written to
# out/play/playtest-cjack.sh each run; tools/ is not changed.
set -u
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
mkdir -p "$REPO/out/play"
OUT="$REPO/out/play/playtest-cjack.sh"
sed -e 's/nolf2cmds\.txt/cjackcmds.txt/g' \
    -e 's#out/nolf2-legacy-release#out/cjack-legacy-release#g' \
    -e 's#Worlds\\\\RetailSinglePlayer\\\\C01S01#Worlds\\\\RetailSinglePlayer\\\\M01S01#' \
    -e 's#^REPO=.*#REPO="'"$REPO"'"#' \
    "$REPO/tools/playtest.sh" > "$OUT"
exec bash "$OUT" "$@"
