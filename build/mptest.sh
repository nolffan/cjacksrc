#!/usr/bin/env bash
# The multiplayer runtime test: does the dedicated server host a game, and
# does a client join it, under Wine?
#
#   build/mptest.sh [gamedir] [seconds]      (in the build container;
#                                             `make mptest` runs it)
#
# gamedir: a game directory as build/all.sh assembles it (default
#          out/cjack-legacy-release); seconds: how long the client plays
#          before judging (default 60).
#
# Runs in out/play/<name>/ (<name>: the game directory's basename with -mp,
# or $MPTEST_NAME), binaries copied and data linked, as build/playtest.sh's
# runs are, on a virtual display of 640x480 (the client runs full screen:
# the engine wants a display mode of its window's size) with a Wine prefix
# of its own (out/play/<name>/wine). The registry gets what the game's setup
# and 1.1 update write (HKLM\SOFTWARE\Monolith Productions\Contract Jack\1.0:
# InstallDir, NetRegionCode EN, UpdateNum 1, UpdateCommandLine0
# "-rez Update_v1x1.rez"). The client starts once to its menu for 30
# seconds: it writes the multiplayer missions file the server reads
# (retail's MPMissions.txt, from the worlds in the rez files). The server
# then starts on a profile of its own (Profiles/mptest.txt: deathmatch --
# Contract J.A.C.K. ships no co-operative worlds -- LAN only, port 27888,
# dedicated; its campaign that file's mission 12 (DM_VOLKOVSLAIR in ours); the
# profile's bools are ButeMgr's TRUE/FALSE) with its error log on
# (server.log); once it has had 60 seconds, a client starts with
# "+join 127.0.0.1:27888
# +gametype Deathmatch". Writes
#   server.png / client.png   the display before the client and at the end
#   server.log                the server's console
#   error.log                 the client's console (+ErrorLog 1)
#   wine-server.log, wine-client.log
#   windows.txt               the visible windows' titles at the end
#   bt.txt                    with MPTEST_BT=1: winedbg's backtraces of both
#                             processes at the end
#   result.txt                SERVER/CLIENT ALIVE or EXITED
# and prints result.txt.
set -u
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
for t in wine xvfb-run xdotool convert; do
    command -v $t >/dev/null 2>&1 || { echo "mptest.sh: no $t here -- run it in the build container"; exit 2; }
done
abs() { case "$1" in /*) echo "$1" ;; *) echo "$REPO/$1" ;; esac; }
GAME="$(abs "${1:-out/cjack-legacy-release}")"
SECS="${2:-60}"
for f in Lithtech.exe server.dll ContractJackServer.exe cjackcmds.txt autoexec.cfg.retail Default.prf.retail; do
    [ -f "$GAME/$f" ] || { echo "mptest.sh: $GAME/$f missing"; exit 2; }
done
NAME="${MPTEST_NAME:-$(basename "$GAME")-mp}"
RUN="$REPO/out/play/$NAME"
rm -rf "$RUN"; mkdir -p "$RUN/Profiles/mptest/DM_Campaigns" "$RUN/Save"
REL="$(python3 -c 'import os,sys; print(os.path.relpath(sys.argv[1], sys.argv[2]))' "$GAME" "$RUN")"
for f in "$GAME"/*; do
    b=$(basename "$f")
    case "$b" in autoexec.cfg|Save|Profiles|cjackcmds.txt|MANIFEST.txt) continue ;; esac
    if [ -f "$f" ] && [ "$(head -c 2 "$f" | tr -d '\0')" = MZ ]; then cp "$f" "$RUN/$b"
    else ln -s "$REL/$b" "$RUN/$b"; fi
done
cp "$GAME/autoexec.cfg.retail" "$RUN/autoexec.cfg"
cp "$GAME/Default.prf.retail" "$RUN/Profiles/Default.prf"
printf '[Multiplayer]\r\nGameType = 2\r\nPort = 27888\r\nLANOnly = TRUE\r\nDedicated = TRUE\r\nUsePassword = FALSE\r\n' > "$RUN/Profiles/mptest.txt"
printf '[MissionList]\r\nMission0=12\r\nLoopMissions=TRUE\r\n' > "$RUN/Profiles/mptest/DM_Campaigns/default_campaign.txt"
printf '%s ++screenwidth 640 ++screenheight 480 ++bitdepth 32 ++windowed 0 +DisableMovies 1 +ErrorLog 1 +AlwaysFlushLog 1 +join 127.0.0.1:27888 +gametype Deathmatch\r\n' \
    "$(tr -d '\r\n' < "$GAME/cjackcmds.txt")" > "$RUN/cjackcmds.txt"
printf '%s ++screenwidth 640 ++screenheight 480 ++bitdepth 32 ++windowed 0 +DisableMovies 1\r\n' \
    "$(tr -d '\r\n' < "$GAME/cjackcmds.txt")" > "$RUN/cjackcmds-menu.txt"

cd "$RUN" || exit 2
if [ ! -e /dev/snd ] && [ -f /usr/share/alsa/alsa.conf ]; then
    printf '</usr/share/alsa/alsa.conf>\npcm.!default { type null }\n' > asound.conf
    export ALSA_CONFIG_PATH="$RUN/asound.conf"
fi
export WINRUN="Z:$(echo "$RUN" | tr / '\\')"
export WINEPREFIX="$RUN/wine" WINEDEBUG="${MPTEST_WINEDEBUG:--all,+seh,+msgbox}" WINEDLLOVERRIDES="winedbg.exe=d"
xvfb-run -a -s '-screen 0 640x480x24' bash -c '
    wineboot -i >/dev/null 2>&1
    K="HKLM\\Software\\Monolith Productions\\Contract Jack\\1.0"
    wine reg add "$K" /v InstallDir /t REG_SZ /d "$WINRUN" /f >/dev/null 2>&1
    wine reg add "$K" /v NetRegionCode /t REG_SZ /d EN /f >/dev/null 2>&1
    wine reg add "$K" /v UpdateNum /t REG_DWORD /d 1 /f >/dev/null 2>&1
    wine reg add "$K" /v UpdateCommandLine0 /t REG_SZ /d "-rez Update_v1x1.rez" /f >/dev/null 2>&1
    # The client writes the multiplayer missions file (MPMissions.txt,
    # from the worlds in the rez files) when it starts; the server reads it.
    wine Lithtech.exe -cmdfile cjackcmds-menu.txt > wine-menu.log 2>&1 &
    sleep 30
    wineserver -k 2>/dev/null; sleep 2
    wine ContractJackServer.exe -profile mptest +errorlog 1 > wine-server.log 2>&1 & S=$!
    sleep 60
    xwd -root -silent | convert xwd:- server.png 2>/dev/null
    wine Lithtech.exe -cmdfile cjackcmds.txt > wine-client.log 2>&1 & C=$!
    sleep '"$SECS"'
    xwd -root -silent | convert xwd:- client.png 2>/dev/null
    for w in $(xdotool search --onlyvisible --name . 2>/dev/null); do xdotool getwindowname $w; done > windows.txt 2>&1
    if [ -n "${MPTEST_BT:-}" ]; then
        for p in $(WINEDEBUG=-all WINEDLLOVERRIDES= wine winedbg --command "info proc" 2>/dev/null | grep -i "Lithtech\|ContractJackServer" | awk "{print \$1}"); do
            printf "bt all\ndetach\nquit\n" | WINEDEBUG=-all WINEDLLOVERRIDES= wine winedbg 0x$p
        done > bt.txt 2>&1
    fi
    { kill -0 $S 2>/dev/null && echo "SERVER ALIVE" || echo "SERVER EXITED"
      kill -0 $C 2>/dev/null && echo "CLIENT ALIVE" || echo "CLIENT EXITED"; } > result.txt
    wineserver -k 2>/dev/null
'
cat result.txt
grep -q "SERVER ALIVE" result.txt && grep -q "CLIENT ALIVE" result.txt
