#!/usr/bin/env bash
# The game built from the same source with project defines that the release
# build never has -- never for measuring: progress is only ever scored on the
# release build (`make`, `make progress`).
#
#   build/variant.sh debug         make debug          -> out/cjack-legacy-debug/
#   build/variant.sh modern        make modern         -> out/cjack-modern-release/
#   build/variant.sh modern-debug  make modern-debug   -> out/cjack-modern-debug/
#
# (on the host the make targets run this in the build container; inside
# `make shell` run it directly). The result is a game directory like
# out/cjack-legacy-release/.
#
#   debug    CJACK_DEBUG and symbols, for finding out why something goes
#            wrong. Compile + /Z7 /Oy- /D CJACK_DEBUG (debug information,
#            frame pointers kept so a backtrace can walk the stack; every
#            other flag, the optimisation included, is the release one);
#            resource scripts + /d CJACK_DEBUG;
#            link + /DEBUG /OPT:REF /INCREMENTAL:NO (a .pdb per binary, the
#            same unreferenced-code removal as release). The .pdb and .map of
#            every binary go beside it. src/cjacksrc/cjacksrc_debug.h's
#            CJACK_LOG lines go to cjacksrc.log in the game's directory.
#   modern   CJACK_MODERN: the modern build's fixes and features
#            (MODERN.md). Compile + /D CJACK_MODERN, resource scripts
#            + /d CJACK_MODERN. And open-source middleware in place of
#            RAD's (CJACK_MIDDLEWARE=open, build/ext.sh): our Bink player
#            on FFmpeg, built from src/modern/binkw32/ and the ext/ffmpeg
#            submodule, as nolf2src's (MODERN.md X-1).
#   modern-debug   both.
#
# The flags reach every compile and link through build/env.sh's
# CJACKSRC_CFLAGS / CJACKSRC_LDFLAGS, which are empty for every release
# command -- and which build/env.sh refuses outside the trees this script
# makes, so they cannot reach the release build by accident.
#
# It never touches the release build. The source is copied (without any
# build output) into out/build-<variant>/ and built there by the same
# build/all.sh, so src/*/built -- what `make progress` scores -- and
# out/cjack-legacy-release/ are left exactly as they were. The copy keeps its
# own build outputs from one run to the next, and only changed sources are
# rewritten into it, so a rebuild redoes only what changed. The game data
# are extracted into out/<name>/ on its first run (hard links to the
# retail files where they can be) and kept after that. Last, the game directory is packaged as
# out/<name>-private.tar.gz and -public.tar.gz, like the release build's
# (build/package.sh).
set -e
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO"

VARIANT="${1:-}"
case "$VARIANT" in
    debug)        CFLAGS="/Z7 /Oy- /D CJACK_DEBUG"
                  LDFLAGS="/DEBUG /OPT:REF /INCREMENTAL:NO"
                  RCFLAGS="/d CJACK_DEBUG" ;;
    modern)       CFLAGS="/D CJACK_MODERN"
                  LDFLAGS=""
                  RCFLAGS="/d CJACK_MODERN" ;;
    modern-debug) CFLAGS="/Z7 /Oy- /D CJACK_DEBUG /D CJACK_MODERN"
                  LDFLAGS="/DEBUG /OPT:REF /INCREMENTAL:NO"
                  RCFLAGS="/d CJACK_DEBUG /d CJACK_MODERN" ;;
    *) echo "usage: build/variant.sh debug|modern|modern-debug" >&2; exit 2 ;;
esac
# the game directory it builds: out/cjack-<legacy|modern>-<release|debug>/
case "$VARIANT" in
    debug)        NAME=cjack-legacy-debug ;;
    modern)       NAME=cjack-modern-release ;;
    modern-debug) NAME=cjack-modern-debug ;;
esac

if ! command -v wine >/dev/null 2>&1; then
    echo "error: no wine here -- run \`make $VARIANT\` from the repository root, or" >&2
    echo "       run this inside \`make shell\`." >&2
    exit 1
fi

TREE="$REPO/out/build-$VARIANT"
echo "== $VARIANT tree: src/ build/ tools/ -> out/build-$VARIANT/ (sources only; its outputs kept)"
mkdir -p "$TREE"
python3 tools/releasesrc.py mirror "$REPO/src" "$TREE/src"
rm -rf "$TREE/build" "$TREE/tools"
tar -C "$REPO" -cf - --exclude='__pycache__' --exclude=.git build tools | tar -C "$TREE" -xf -
# the compilers, and the retail game where build/retail.sh looks for it
[ -L "$TREE/toolchains" ] || ln -s ../../toolchains "$TREE/toolchains"
[ -L "$TREE/rez" ] || ln -s ../../rez "$TREE/rez"
# the modern build's open-source middleware (build/ext.sh), built once into
# out/ext/ for every variant
[ -L "$TREE/ext" ] || ln -s ../../ext "$TREE/ext"

export CJACKSRC_CFLAGS="$CFLAGS"
export CJACKSRC_LDFLAGS="$LDFLAGS"
export CJACKSRC_RCFLAGS="$RCFLAGS"
export CJACK_GAME_OUT="$REPO/out/$NAME"
export CJACK_PACKAGE=0
case "$VARIANT" in modern*) export CJACK_MIDDLEWARE=open CJACK_EXT_OUT="$REPO/out/ext" ;; esac
bash "$TREE/build/all.sh"

case "$VARIANT" in *debug)
    echo
    echo "== symbols: each built binary's .pdb (and .map) beside it in out/$NAME/"
    # MANIFEST.txt has `built <source>\t<installed path>` for every binary we
    # built; its .pdb is the link output's name with .pdb, and the debugger
    # looks for it by that name in the binary's own directory.
    grep '^built ' "$CJACK_GAME_OUT/MANIFEST.txt" | sort -u |
    while IFS=$'\t' read -r src dest; do
        src="${src#built }"
        pdb="$TREE/${src%.*}.pdb"
        if [ -f "$pdb" ]; then
            cp "$pdb" "$CJACK_GAME_OUT/$(dirname "$dest")/"
            echo "   $(dirname "$dest")/$(basename "$pdb")"
        else
            echo "   (no .pdb for $dest)"
        fi
        # the linker's map, for addresses no .pdb names (see cjacksrc_crash.h)
        map="$TREE/${src%.*}.map"
        [ -f "$map" ] && cp "$map" "$CJACK_GAME_OUT/$(dirname "$dest")/"
    done ;;
esac
echo
echo "== package: out/$NAME/ -> out/$NAME-private.tar.gz, -public.tar.gz"
bash "$TREE/build/package.sh" "out/$NAME"
echo "== done: out/$NAME/"
