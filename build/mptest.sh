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
# "-rez Update_v1x1.rez", and the CDKey the multiplayer screen stores:
# $MPTEST_CDKEY, default a key of the right form; MPTEST_CDKEY=none writes
# none, for the modern build, which checks no key: MODERN.md APPS-6). The
# modern build reads the user's key, not this one (APPS-7), and needs none
# of it. The client starts once
# to its menu for 30
# seconds: it writes the multiplayer missions file the server reads
# (retail's MPMissions.txt, from the worlds in the rez files). The server
# then starts on a profile of its own (Profiles/mptest.txt: deathmatch --
# Contract J.A.C.K. ships no co-operative worlds -- LAN only, port 27888,
# dedicated; its campaign that file's mission $MPTEST_MISSION, by default
# the first world of the game type, MPTEST_TYPE dm, tdm, dd or de); the
# profile's bools are ButeMgr's TRUE/FALSE) with its error log on
# (server.log); once it has had 60 seconds, a client starts with
# "+join 127.0.0.1:27888
# +gametype <the type>" (out/play/<name>-mp-<type>/ for all but dm). Writes
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
# The game type: dm (deathmatch, the default), tdm (team deathmatch), dd
# (Doomsday), de (Demolition): the profile's GameType, its campaign
# directory, the worlds it takes (by their names' prefix) and the client's
# +gametype.
TYPE="${MPTEST_TYPE:-dm}"
case "$TYPE" in
    dm)  GT=2; CDIR=DM_Campaigns;  PFX=DM_; GTNAME=Deathmatch ;;
    tdm) GT=3; CDIR=TDM_Campaigns; PFX=DM_; GTNAME=TeamDeathmatch ;;
    dd)  GT=4; CDIR=DD_Campaigns;  PFX=DD_; GTNAME=DoomsDay ;;
    de)  GT=5; CDIR=DE_Campaigns;  PFX=DE_; GTNAME=Demolition ;;
    *)   echo "mptest.sh: MPTEST_TYPE is dm, tdm, dd or de"; exit 2 ;;
esac
SUFFIX=""; [ "$TYPE" = dm ] || SUFFIX="-$TYPE"
NAME="${MPTEST_NAME:-$(basename "$GAME")-mp$SUFFIX}"
RUN="$REPO/out/play/$NAME"
rm -rf "$RUN"; mkdir -p "$RUN/Profiles/mptest/$CDIR" "$RUN/Save"
REL="$(python3 -c 'import os,sys; print(os.path.relpath(sys.argv[1], sys.argv[2]))' "$GAME" "$RUN")"
for f in "$GAME"/*; do
    b=$(basename "$f")
    case "$b" in autoexec.cfg|Save|Profiles|cjackcmds.txt|MANIFEST.txt) continue ;; esac
    if [ -f "$f" ] && [ "$(head -c 2 "$f" | tr -d '\0')" = MZ ]; then cp "$f" "$RUN/$b"
    else ln -s "$REL/$b" "$RUN/$b"; fi
done
cp "$GAME/autoexec.cfg.retail" "$RUN/autoexec.cfg"
cp "$GAME/Default.prf.retail" "$RUN/Profiles/Default.prf"
printf '[Multiplayer]\r\nGameType = %s\r\nPort = 27888\r\nLANOnly = TRUE\r\nDedicated = TRUE\r\nUsePassword = FALSE\r\n' "$GT" > "$RUN/Profiles/mptest.txt"
# The campaign: mission $MPTEST_MISSION of the missions file, or else the
# first whose world's name has the type's prefix (once the client has
# written the file; mkcampaign.sh runs between the two starts).
cat > "$RUN/mkcampaign.sh" <<MKC
m="\${MPTEST_MISSION:-}"
if [ -z "\$m" ]; then
    m=\$(tr -d '\r' < MPMissions.txt | awk -v p='$PFX' 'BEGIN { p = toupper(p) }
        /^\[Mission/ { gsub(/[^0-9]/, ""); n = \$0 }
        /^Level0=/ && f == "" && index(toupper(\$0), "\\\\" p) { f = n }
        END { print f }')
fi
printf '[MissionList]\\r\\nMission0=%s\\r\\nLoopMissions=TRUE\\r\\n' "\${m:-0}" > "Profiles/mptest/$CDIR/default_campaign.txt"
echo "mptest: $TYPE on mission \${m:-0}" > campaign.txt
MKC
printf '%s ++screenwidth 640 ++screenheight 480 ++bitdepth 32 ++windowed 0 +DisableMovies 1 +ErrorLog 1 +AlwaysFlushLog 1 +join 127.0.0.1:27888 +gametype %s\r\n' \
    "$(tr -d '\r\n' < "$GAME/cjackcmds.txt")" "$GTNAME" > "$RUN/cjackcmds.txt"
printf '%s ++screenwidth 640 ++screenheight 480 ++bitdepth 32 ++windowed 0 +DisableMovies 1\r\n' \
    "$(tr -d '\r\n' < "$GAME/cjackcmds.txt")" > "$RUN/cjackcmds-menu.txt"

cd "$RUN" || exit 2
if [ ! -e /dev/snd ] && [ -f /usr/share/alsa/alsa.conf ]; then
    printf '</usr/share/alsa/alsa.conf>\npcm.!default { type null }\n' > asound.conf
    export ALSA_CONFIG_PATH="$RUN/asound.conf"
fi
# A CD key of the right form (GameSpy.dll's IsCDKeyValid; this one is from
# build/gamespytest.sh's random keys, not a sold one): with none, the
# client's challenge response is empty and the server rejects the join.
export MPTEST_CDKEY="${MPTEST_CDKEY:-BUN6-RYX3-DUZ9-MED2-7352}"
export WINRUN="Z:$(echo "$RUN" | tr / '\\')"
export WINEPREFIX="$RUN/wine" WINEDEBUG="${MPTEST_WINEDEBUG:--all,+seh,+msgbox}" WINEDLLOVERRIDES="winedbg.exe=d"
# MPTEST_WINEDBG=1: a crash's minidump (winedbg, through AeDebug) goes to
# crash.mdmp and its backtrace to crash.txt; Wine's crash debugger is off
# otherwise (it would wait on a dialog)
export MPTEST_WINEDBG="${MPTEST_WINEDBG:-}"
[ -n "$MPTEST_WINEDBG" ] && export WINEDLLOVERRIDES=""
xvfb-run -a -s '-screen 0 640x480x24' bash -c '
    wineboot -i >/dev/null 2>&1
    K="HKLM\\Software\\Monolith Productions\\Contract Jack\\1.0"
    wine reg add "$K" /v InstallDir /t REG_SZ /d "$WINRUN" /f >/dev/null 2>&1
    wine reg add "$K" /v NetRegionCode /t REG_SZ /d EN /f >/dev/null 2>&1
    if [ -n "$MPTEST_WINEDBG" ]; then
        # A crash starts winedbg (Wine, as Windows, through AeDebug), which
        # writes a minidump, crash.mdmp; its backtrace goes to crash.txt
        A="HKLM\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug"
        wine reg add "$A" /v Debugger /t REG_SZ /d "C:\\windows\\system32\\winedbg.exe --minidump $WINRUN\\crash.mdmp %ld %ld" /f >/dev/null 2>&1
        wine reg add "$A" /v Auto /t REG_SZ /d 1 /f >/dev/null 2>&1
        wine reg add "HKCU\\Software\\Wine\\WineDbg" /v ShowCrashDialog /t REG_DWORD /d 0 /f >/dev/null 2>&1
    fi
    [ "$MPTEST_CDKEY" = none ] || wine reg add "$K" /v CDKey /t REG_SZ /d "$MPTEST_CDKEY" /f >/dev/null 2>&1
    wine reg add "$K" /v UpdateNum /t REG_DWORD /d 1 /f >/dev/null 2>&1
    wine reg add "$K" /v UpdateCommandLine0 /t REG_SZ /d "-rez Update_v1x1.rez" /f >/dev/null 2>&1
    # The client writes the multiplayer missions file (MPMissions.txt,
    # from the worlds in the rez files) when it starts; the server reads it.
    wine Lithtech.exe -cmdfile cjackcmds-menu.txt > wine-menu.log 2>&1 &
    sleep 30
    wineserver -k 2>/dev/null; sleep 2
    bash mkcampaign.sh
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
    if [ -n "$MPTEST_WINEDBG" ] && [ -f crash.mdmp ]; then
        printf "bt\nbt all\ninfo share\nquit\n" | wine winedbg crash.mdmp > crash.txt 2>&1
    fi
    wineserver -k 2>/dev/null
'
cat result.txt
grep -q "SERVER ALIVE" result.txt && grep -q "CLIENT ALIVE" result.txt
