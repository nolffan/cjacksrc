#!/usr/bin/env bash
# The single-player campaign under Wine, level by level: every world of
# Contract J.A.C.K.'s missions (Attributes/Missions.txt, in order), each
# loaded on its own from the command line and played by
# build/playtest-level.txt (into control, moving, firing, a quicksave and a
# quickload).
#
#   build/campaign.sh [gamedir] [world...]     (in the build container;
#                                               `make campaign` runs it)
#
# gamedir: default out/cjack-legacy-release; worlds: default all of them
# (M01S01 ... M07S04, EPILOGUE; give the bare names). Each run is
# build/playtest.sh's, named <gamedir's basename>-<world> under out/play/;
# out/play/campaign-<gamedir's basename>.txt collects one result line per
# world. With CAMPAIGN_CONTROL=<other run prefix> each world's shots are
# also compared with that prefix's run of the same world
# (playtest.sh compare: RMSE per shot, compare-*.png).
set -u
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
GAME="${1:-out/cjack-legacy-release}"; shift || true
WORLDS=("$@")
[ ${#WORLDS[@]} -gt 0 ] || WORLDS=(M01S01 M02S01 M03S01 M03S02 M04S01 M04S02 M04S03 M05S01 M05S02 M06S01 M07S01 M07S02 M07S03 M07S04 EPILOGUE)
BASE="$(basename "$GAME")"
SUM="$REPO/out/play/campaign-$BASE.txt"
mkdir -p "$REPO/out/play"; : > "$SUM"
for w in "${WORLDS[@]}"; do
    name="$BASE-$w"
    r=$(PLAYTEST_NAME="$name" PLAYTEST_SCRIPT="${CAMPAIGN_SCRIPT:-build/playtest-level.txt}" \
        bash "$REPO/build/playtest.sh" "$GAME" "Worlds\\RetailSinglePlayer\\$w" 5 2>&1 | tail -1)
    line=$(printf '%-10s %s' "$w" "$r")
    if [ -n "${CAMPAIGN_CONTROL:-}" ] && [ -d "$REPO/out/play/${CAMPAIGN_CONTROL}-$w/shots" ]; then
        cmp=$(bash "$REPO/out/play/playtest-cjack.sh" compare "$name" "${CAMPAIGN_CONTROL}-$w" 2>/dev/null \
              | awk '{printf "%s=%s ", $1, $2}')
        line="$line | $cmp"
    fi
    echo "$line" | tee -a "$SUM"
done
