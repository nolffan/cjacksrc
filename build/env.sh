# Source this to get the Visual C++ .NET 2003-under-Wine build environment. It runs inside the
# build container (docker/Dockerfile; `make shell` gives you one), where Wine
# is on PATH and the toolchain is toolchains/ in this checkout
# (toolchains/README.md). The compile and link command lines themselves are
# build/build.mk's.

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
for f in msvc-7.10-3077/bin/cl.exe dxsdk-8.1/include/d3d8.h; do
    if [ ! -f "$REPO/toolchains/$f" ]; then
        echo "error: toolchains/$f not found -- the toolchain is missing (toolchains/README.md)" >&2
        return 1 2>/dev/null || exit 1
    fi
done
if ! command -v wine >/dev/null 2>&1; then
    echo "error: wine is not on PATH -- run this inside the build container (make shell)" >&2
    return 1 2>/dev/null || exit 1
fi
export WINEDEBUG=-all
export WINEDLLOVERRIDES="mscoree,mshtml="
# the variant macros tools/releasesrc.py resolves, and the project's own
# header directory tools/mkdeps.py follows
export RELEASESRC_VARIANTS="CJACK_MODERN CJACK_DEBUG"
export MKDEPS_PROJECT_INCLUDE=src/cjacksrc

# Extra flags for every compile and every link, for the debug and modern
# builds (build/variant.sh sets them). Empty otherwise -- and the release
# build's command lines are then exactly what the project files say.
export CJACKSRC_CFLAGS="${CJACKSRC_CFLAGS:-}"
export CJACKSRC_LDFLAGS="${CJACKSRC_LDFLAGS:-}"
export CJACKSRC_RCFLAGS="${CJACKSRC_RCFLAGS:-}"    # resource scripts: /d NAME
# They are only allowed in the trees build/variant.sh makes (out/build-*/):
# anywhere else they would put CJACK_DEBUG or CJACK_MODERN code into the
# objects `make progress` scores, which must only ever be the release build.
if [ -n "$CJACKSRC_CFLAGS$CJACKSRC_LDFLAGS$CJACKSRC_RCFLAGS" ]; then
    case "$REPO" in
        */out/build-*) ;;
        *) echo "error: CJACKSRC_CFLAGS / LDFLAGS / RCFLAGS are set, but this is not a" >&2
           echo "       build/variant.sh tree ($REPO). Unset them; use make debug/modern." >&2
           return 1 2>/dev/null || exit 1 ;;
    esac
fi

# The release build compiles src/ WITHOUT its CJACK_MODERN /
# CJACK_DEBUG blocks, not merely with them switched off: a block the
# compiler skips still moves everything below it, and VC6 lets the file's
# layout decide where some out-of-line inline functions go, so enough
# skipped blocks can change the release binary (tools/releasesrc.py; a
# lesson of VC6's, kept until VC .NET 2003 is shown not to need it).
# build/compile.sh calls this first, with its own arguments: in a checkout it
# brings out/release-tree/ up to date -- src/ with those blocks resolved as
# the release build sees them, the same build/ and tools/, this checkout's
# build outputs -- runs the script there, copies the build outputs back into
# this checkout's src/ (where `make progress` and the checks read them) and
# exits with the script's status. In a build/variant.sh tree (out/build-*/,
# which must see the blocks), or inside the release tree, it does nothing.
cjacksrc_release_tree() {
    [ -n "${CJACKSRC_IN_RELEASE_TREE:-}" ] && return 0
    case "$REPO" in */out/build-*) return 0 ;; esac
    local tree="$REPO/out/release-tree" script rc=0
    script="$(basename "${BASH_SOURCE[1]}")"
    mkdir -p "$tree"
    python3 "$REPO/tools/releasesrc.py" sync "$REPO/src" "$tree/src" || exit 1
    python3 "$REPO/tools/releasesrc.py" outputs "$REPO/src" "$tree/src" || exit 1
    rm -rf "$tree/build" "$tree/tools"
    tar -C "$REPO" -cf - --exclude='__pycache__' --exclude=.git build tools | tar -C "$tree" -xf -
    [ -L "$tree/toolchains" ] || ln -s ../../toolchains "$tree/toolchains"
    [ -L "$tree/rez" ] || ln -s ../../rez "$tree/rez"
    ( cd "$tree" && CJACKSRC_IN_RELEASE_TREE=1 bash "build/$script" "$@" ) || rc=$?
    python3 "$REPO/tools/releasesrc.py" outputs "$tree/src" "$REPO/src" || exit 1
    exit $rc
}

# Compile and link through build/build.mk, in this tree (run it after
# cjacksrc_release_tree): every listed target, CJACKSRC_JOBS at a time
# (default: one per CPU), each job's output kept together.
cjacksrc_make() {
    make -C "$REPO" -f build/build.mk --no-print-directory -Otarget \
        -j"${CJACKSRC_JOBS:-$(nproc)}" "$@"
}

# ---------------------------------------------------------------------------
# The tools by hand, for the one-object loop and experiments
# (build/engine.sh, tools/guides/02, 10): the compiler, linker, librarian
# and resource compiler build/build.mk uses, in the same environment --
# src/ as drive S:, toolchains/ as T:, the DirectX 8.1 SDK first on the
# INCLUDE and LIB paths, src/cjacksrc last -- so a command line copied out of
# an object's <object>.cmd gives the same object. (A compile's .cmd also
# starts with `cd <dir> &&`: run it from that directory.) They use their own
# Wine prefix, out/tools-wine, so they never touch the build's.
#
#   run_cl /nologo /O2 /c foo.cpp       cl 13.10.3077 (VC .NET 2003)
#   run_link, run_lib, run_rc           link/lib 7.10.3077, rc 5.2
#   cjacksrc_wpath <path>               the Windows path the tools see
TC="$REPO/toolchains"
VC71="$TC/msvc-7.10-3077"
DXSDK="$TC/dxsdk-8.1"
cjacksrc_wpath() {
    local p
    p="$(realpath -m "$1")"
    case "$p" in
        "$REPO/src"|"$REPO/src"/*)      p="s:${p#"$REPO/src"}" ;;
        "$TC"|"$TC"/*)                  p="t:${p#"$TC"}" ;;
        *)                              p="z:$p" ;;
    esac
    echo "${p//\//\\}"
}
cjacksrc_tools_env() {   # the prefix, made the first time, with S: and T:
    export WINEPREFIX="$REPO/out/tools-wine"
    if [ ! -f "$WINEPREFIX/.ready" ]; then
        mkdir -p "$WINEPREFIX"
        wine wineboot -i >/dev/null 2>&1 || true
        wineserver -w || true
        touch "$WINEPREFIX/.ready"
    fi
    ln -sfn "$REPO/src" "$WINEPREFIX/dosdevices/s:"
    ln -sfn "$TC" "$WINEPREFIX/dosdevices/t:"
    local v d
    v="$(cjacksrc_wpath "$VC71")"; d="$(cjacksrc_wpath "$DXSDK")"
    export WINEPATH="$v\\bin"
    export INCLUDE="$d\\include;$v\\include;$v\\atlmfc\\include;$v\\PlatformSDK\\include;$(cjacksrc_wpath "$REPO/src/cjacksrc")"
    export LIB="$d\\lib;$v\\lib;$v\\atlmfc\\lib;$v\\PlatformSDK\\lib"
}
# The variant flags go after the command's own (/Oy- must follow the /O2
# that implies it), but before a /link, after which the line is the linker's.
run_cl() {
    local a linked=0 args=()
    for a in "$@"; do
        if [ $linked = 0 ] && [ "${a,,}" = "/link" ]; then
            args+=($CJACKSRC_CFLAGS); linked=1
        fi
        args+=("$a")
    done
    [ $linked = 1 ] || args+=($CJACKSRC_CFLAGS)
    ( cjacksrc_tools_env && wine "$VC71/bin/cl.exe" "${args[@]}" )
}
run_link() { ( cjacksrc_tools_env && wine "$VC71/bin/link.exe" $CJACKSRC_LDFLAGS "$@" ); }
run_lib()  { ( cjacksrc_tools_env && wine "$VC71/bin/lib.exe" "$@" ); }
run_rc()   { ( cjacksrc_tools_env && wine "$VC71/bin/rc.exe" $CJACKSRC_RCFLAGS "$@" ); }
