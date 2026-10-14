#!/usr/bin/env bash
# THE build: Contract J.A.C.K. in out/cjack-legacy-release/, with every
# binary compiled here and the data taken from the retail game.
#
#   make                    (on the host: builds the docker image, installs
#                            and checks the retail game, runs this in a
#                            container)
#   build/all.sh            (inside the build container, e.g. `make shell`)
#
# Needs: the toolchain (toolchains/, see toolchains/README.md), Wine (the
# build container has it), and the retail game, Contract J.A.C.K. (Europe)
# with the English 1.1 update, as the rez/ submodule installs it
# (rez/games/contract-jack-eu-1.1/, build/retail.sh). It only reads that
# directory.
#
# The steps, in order:
#   1. resources          the game's data from the retail game: the .rez
#                         archives (with the retail game DLLs inside them
#                         zeroed), the movies, the shipped autoexec.cfg and
#                         default profile. No binary.
#   2. third-party runtime  Bink (binkw32.dll) and the Microsoft C++ and MFC
#                         7.1 runtime DLLs, from the retail game: licensed
#                         binaries with no source.
#   3. compile            every workspace build/build.mk knows of that is in
#                         the tree, all at once (one job per CPU;
#                         CJACKSRC_JOBS=<n> to change it) and only what
#                         changed since the last build.
#   4. assemble           the built binaries into out/cjack-legacy-release/:
#                         the game DLLs in the custom/ rez directory, last in
#                         the rez stack (cjackcmds.txt); the engine, the
#                         dedicated server and the launcher in the game's
#                         root. A binary whose source is not here yet
#                         (docs/PLAN.md) is left out, and said so: the game
#                         runs only once every one of them is built.
#   5. audit              every PE file in out/cjack-legacy-release/ listed
#                         with its origin; the build fails if any is
#                         unaccounted for or if a .rez archive still holds
#                         one.
#   6. package            out/cjack-legacy-release-private.tar.gz: the game
#                         directory as one file, and -public.tar.gz: the same
#                         without the retail game's files, which its install
#                         scripts copy out of the player's own disc images
#                         and 1.1 update (build/package.sh).
set -e
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Wine and the toolchain live in the build container: on the host, use make.
if ! command -v wine >/dev/null 2>&1; then
    echo "error: no wine here -- run \`make\` from the repository root (it runs this" >&2
    echo "       script in the build container), or run it inside \`make shell\`." >&2
    exit 1
fi
export WINEDEBUG=-all

cd "$REPO"
source build/retail.sh
OUT="${CJACK_GAME_OUT:-out/cjack-legacy-release}"
RETAIL="$(retail_dir)"
retail_verify
T0=$(date +%s)
step() { echo; echo "######## $* ($(( $(date +%s) - T0 ))s)"; }
B=src/Game/built/Final
E=src/Engine/built/Final
BUILT=(
    "$B/ClientShellDLL/TO2/CShell.dll"  custom/cshell.dll
    "$B/ObjectDLL/TO2/Object.lto"       custom/object.lto
    "$B/ClientFxDLL/ClientFx.fxd"       custom/clientfx.fxd
    "$B/ClientRes/TO2/CRes.dll"         custom/cres.dll
    "$B/ServerRes/TO2/SRes.dll"         custom/sres.dll
    "$B/ServerApp/ContractJackServer.exe"  ContractJackServer.exe
    "$B/Launcher/ContractJack.exe"      ContractJack.exe
    "$B/libs/GameSpy/GameSpy.dll"       GameSpy.dll
    "$E/Lithtech/Lithtech.exe"          Lithtech.exe
    "$E/server/server.dll"              server.dll
    "$E/snddrv/SndDrv.dll"              SndDrv.dll
    "$E/ltmsg/ltmsg.dll"                ltmsg.dll
)

step "1. resources: game data from $RETAIL -> $OUT"
python3 tools/retailextract.py resources "$RETAIL" "$OUT"
# autoexec.cfg and the default profile start as the shipped ones (the game
# rewrites them); a rebuild keeps the player's.
[ -f "$OUT/autoexec.cfg" ] || cp "$OUT/autoexec.cfg.retail" "$OUT/autoexec.cfg"
mkdir -p "$OUT/Save" "$OUT/custom" "$OUT/Profiles"
[ -f "$OUT/Profiles/Default.prf" ] || cp "$OUT/Default.prf.retail" "$OUT/Profiles/Default.prf"
bash build/cjackcmds.sh > "$OUT/cjackcmds.txt"

step "2. THIRD-PARTY RUNTIME: Bink and the Microsoft runtime from $RETAIL"
python3 tools/retailextract.py thirdparty "$RETAIL" "$OUT"

step "3. compile: the workspaces (build/compile.sh: build/build.mk, ${CJACKSRC_JOBS:-$(nproc)} jobs)"
bash build/compile.sh all
missing=()
for ((i = 0; i < ${#BUILT[@]}; i += 2)); do
    [ -f "${BUILT[i]}" ] || missing+=("${BUILT[i+1]}")
done

step "4. assemble: the built binaries -> $OUT"
for ((i = 0; i < ${#BUILT[@]}; i += 2)); do
    [ -f "${BUILT[i]}" ] || continue
    cp "${BUILT[i]}" "$OUT/${BUILT[i+1]}"
    printf 'built %s\t%s\n' "${BUILT[i]}" "${BUILT[i+1]}" >> "$OUT/MANIFEST.txt"
    echo "   ${BUILT[i+1]}  <-  ${BUILT[i]}"
done
# one row per file: the manifest is rewritten by the next step's tool
python3 - "$OUT/MANIFEST.txt" <<'PY'
import sys
p = sys.argv[1]
rows = {}
for line in open(p).read().splitlines():
    if "\t" in line and not line.startswith("#"):
        origin, path = line.split("\t", 1)
        rows[path] = origin
with open(p, "w") as f:
    f.write("# origin<TAB>path, one row per file build/all.sh put here\n")
    for path in sorted(rows, key=str.lower):
        f.write(f"{rows[path]}\t{path}\n")
PY

step "5. audit: every PE file in $OUT"
python3 tools/retailextract.py audit "$OUT"

# (CJACK_PACKAGE=0 skips it: build/variant.sh packages its game directory
# itself)
if [ "${CJACK_PACKAGE:-1}" = 1 ]; then
    step "6. package: $OUT -> $OUT-private.tar.gz, -public.tar.gz"
    bash "$REPO/build/package.sh" "$OUT"
fi

if [ ${#missing[@]} -gt 0 ]; then
    step "done: $OUT ($(( $(date +%s) - T0 ))s) -- not playable yet: no source yet for ${missing[*]} (docs/PLAN.md)"
else
    step "done: $OUT ($(( $(date +%s) - T0 ))s)"
fi
