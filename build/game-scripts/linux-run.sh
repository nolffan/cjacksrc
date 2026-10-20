#!/bin/sh
# linux-run.sh -- start Contract J.A.C.K. (built from source) with Wine,
# through its launcher, ContractJack.exe (ours, built from
# src/Game/Launcher), the way the retail game starts.
#
#   ./linux-run.sh [launcher args...]
#
# Click "Play" (the first time, the launcher asks for a detail setting
# first). The launcher starts Lithtech.exe with the game's archives; its
# Display and Options buttons set the renderer's resolution and the
# engine's options. Arguments are handed to the launcher, which passes them
# on to the engine after its own (later ones win), e.g.
#   ./linux-run.sh ++windowed 0                  full screen
#   ./linux-run.sh +DisableSound 1               no sound
#   ./linux-run.sh +DisableMovies 0              the intro movies on
#   ./linux-run.sh +JOIN <address>               join a multiplayer game
#   ./linux-run.sh --help                        this text
#
# The game starts in a window (++windowed 1, before the arguments, so
# ./linux-run.sh ++windowed 0 still asks for full screen): the launcher's
# Display has no window mode. The intro movies are off (+DisableMovies 1,
# likewise before the arguments): RAD's binkw32.dll plays them, and its
# movies stall under some Wine setups.
#
# Environment:
#   WINEPREFIX   the Wine prefix to use (default: $HOME/.wine-cjack, created
#                on first run, so your own prefix is left alone)
#   WINE         the wine binary (default: wine)
#   WINEDEBUG    Wine's log channels (default: fixme-all)
#
# The modern build's launcher needs nothing from the registry (it plays the
# game in its own directory and keeps its settings under the user's key:
# MODERN.md APPS-5, APPS-7). The legacy build's is the retail one, which
# reads what the retail installer left in the registry, and this script
# writes that into its Wine prefix, under HKLM\Software\Monolith
# Productions\Contract Jack\1.0:
#   * InstallDir: this directory (the launcher changes to it).
#   * UpdateNum, UpdateCommandLine0: the archive it adds after its own
#     (Game.rez, GameDLL.rez, Sound.rez, custom): the 1.1 update's, as its
#     installer wrote -- and, after it, custom/ again, holding the game DLLs
#     built from source, so that the update's (zeroed) retail DLLs cannot
#     override them (later archives win).
# No disc is needed: the launcher only looks for one to install from.
# Multiplayer asks for a CD key the first time (Multiplayer > CD Key).
# On the first run autoexec.cfg -- the game's settings, rewritten on every
# exit -- starts as a copy of the shipped one (autoexec.cfg.retail).
#
# Sound: the engine's SndDrv.dll plays through DirectSound, so it needs
# Wine's audio working on this machine (winecfg > Audio); +DisableSound 1
# runs without it. The music is DirectMusic, which Wine only partly
# implements; Microsoft's DirectMusic can be installed in the prefix:
#   WINEPREFIX=~/.wine-cjack winetricks dmband dmcompos dmime dmloader \
#       dmscript dmstyle dmsynth dmusic dmusic32 dswave gmdls
set -e
cd "$(dirname "$0")"
case "${1:-}" in
--help|-h) sed -n '2,/^set -e$/{/^set -e$/d;p}' "./${0##*/}" | sed 's/^# \{0,1\}//'; exit 0 ;;
esac
WINEPREFIX="${WINEPREFIX:-$HOME/.wine-cjack}"
WINEDEBUG="${WINEDEBUG:-fixme-all}"
export WINEPREFIX WINEDEBUG
WINE="${WINE:-wine}"
command -v "$WINE" >/dev/null 2>&1 || { echo "linux-run.sh: '$WINE' not found -- install Wine (32-bit support needed)" >&2; exit 1; }
[ -f Game.rez ] || { echo "linux-run.sh: the game's data is not here yet -- run ./linux-install.sh first" >&2; exit 1; }

# The modern build (build/all.sh sets this when it installs the script).
MODERN=

[ -f autoexec.cfg ] || cp autoexec.cfg.retail autoexec.cfg
mkdir -p Save

# a prefix to set up: make it now if it does not exist yet
[ -d "$WINEPREFIX/dosdevices" ] || "$WINE" wineboot -i >/dev/null 2>&1 || true
if [ -z "$MODERN" ]; then
    DIR="$("$WINE" winepath -w "$(pwd)" 2>/dev/null)"
    KEY='HKLM\Software\Monolith Productions\Contract Jack\1.0'
    # Both registry views: in a 64-bit prefix `wine reg` writes the 64-bit
    # one, while ContractJack.exe, a 32-bit program, reads HKLM\Software
    # through the 32-bit one (Wow6432Node). (A 32-bit prefix has one view;
    # the /reg:64 write then fails, harmlessly.)
    reg() { "$WINE" reg add "$@" /reg:32 /f >/dev/null 2>&1
            "$WINE" reg add "$@" /reg:64 /f >/dev/null 2>&1 || true; }
    reg "$KEY" /v InstallDir         /t REG_SZ    /d "$DIR"
    reg "$KEY" /v NetRegionCode      /t REG_SZ    /d EN
    reg "$KEY" /v UpdateNum          /t REG_DWORD /d 1
    reg "$KEY" /v UpdateCommandLine0 /t REG_SZ    /d '-rez Update_v1x1.rez -rez custom'
fi

exec "$WINE" ContractJack.exe ++windowed 1 +DisableMovies 1 "$@"
