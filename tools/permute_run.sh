#!/usr/bin/env bash
# permute_run.sh — drive decomp-permuter for a single Pokemon Snap function.
#
# Usage: tools/permute_run.sh <vram_addr> <size> <seed_c_path> [--threads N] [-- <permuter args>]
# Example: tools/permute_run.sh 0x80131A54 0x38 /tmp/seed_func_80131A54.c
#
# Sets up tools/decomp-permuter/runs/<funcname>/ with base.c, target.s, target.o,
# and a compile.sh that invokes the project's IDO 7.1 with the canonical
# matching CFLAGS. The seed C file is preprocessed into base.c without modifying
# src/, config/, or build/. Asm for the target function is sliced out of the
# splat-emitted asm/ tree and prefixed with the permuter's prelude.inc. The
# script then exec's tools/decomp-permuter/permuter.py and exits 0 if a
# matching permutation was found (asm-permuter prints "found a match!"), 1
# otherwise. Intermediate artefacts live under tools/decomp-permuter/runs/
# (gitignored — never commit them). Override the IDO toolchain with IDO_DIR=
# and the venv with VENV=. Any flags after `--` are forwarded to permuter.py.
set -euo pipefail

if [ "$#" -lt 3 ]; then
    echo "usage: $0 <vram_addr> <size> <seed_c_path> [-- <permuter args>]" >&2
    exit 2
fi

VRAM="$1"
SIZE="$2"
SEED_C="$3"
shift 3

# Forward any remaining args after `--` to permuter.py
PERMUTER_EXTRA_ARGS=()
if [ "$#" -gt 0 ] && [ "$1" = "--" ]; then
    shift
    PERMUTER_EXTRA_ARGS=("$@")
fi

# --- Resolve project paths --------------------------------------------------
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PERMUTER_DIR="${PROJECT_ROOT}/tools/decomp-permuter"
RUNS_DIR="${PERMUTER_DIR}/runs"
VENV="${VENV:-${PROJECT_ROOT}/.venv}"
PYTHON="${VENV}/bin/python"
IDO_VERSION="${IDO_VERSION:-7.1}"
IDO_DIR="${IDO_DIR:-${HOME}/.local/share/ido-recomp/build/${IDO_VERSION}/out}"
IDO_CC="${IDO_DIR}/cc"
MIPS_PREFIX="${MIPS_PREFIX:-mips-linux-gnu-}"

if [ ! -d "${PERMUTER_DIR}" ]; then
    echo "ERROR: decomp-permuter not found at ${PERMUTER_DIR}." >&2
    echo "       git clone https://github.com/simonlindholm/decomp-permuter.git ${PERMUTER_DIR}" >&2
    exit 1
fi
if [ ! -x "${PYTHON}" ]; then
    echo "ERROR: project venv python missing at ${PYTHON}." >&2
    echo "       Run ./tools/setup.sh or 'make setup' first." >&2
    exit 1
fi
if [ ! -x "${IDO_CC}" ]; then
    echo "ERROR: IDO compiler missing at ${IDO_CC}. Set IDO_DIR=." >&2
    exit 1
fi
if [ ! -f "${SEED_C}" ]; then
    echo "ERROR: seed C file not found: ${SEED_C}" >&2
    exit 1
fi

# --- Pre-flight: warn about K&R-style param decls in the seed ---------------
# decomp-permuter routes the seed through pycparser, which does NOT accept
# old-style K&R parameter declarations (the `)\n s32 arg0;\n s32 arg1;\n {`
# shape). It crashes with "does not contain any function!" — confusing
# because the function IS clearly there. Detect the pattern and bail with a
# clear message so the user converts the seed to ANSI before retrying.
# The actual src/ file can stay K&R; only the seed needs to be ANSI.
if awk '
    BEGIN { in_decl = 0 }
    /^[A-Za-z_][A-Za-z_0-9 \*]* func[A-Za-z_0-9]*\([A-Za-z_0-9, ]*\)[[:space:]]*$/ {
        in_decl = 1
        next
    }
    in_decl && /^[[:space:]]*\{/ { in_decl = 0; next }
    in_decl && /^[[:space:]]*(s8|u8|s16|u16|s32|u32|f32|f64|int|char|short|long|signed|unsigned|float|double|void)[ \*]/ {
        print "K&R-style param decl detected at line", NR ": " $0
        exit 1
    }
' "${SEED_C}" 2>&1 | head -5; then
    : # no K&R detected
else
    echo "" >&2
    echo "ERROR: seed file ${SEED_C} contains K&R-style parameter declarations." >&2
    echo "       decomp-permuter's pycparser frontend does not accept K&R syntax;" >&2
    echo "       it fails with 'does not contain any function!'." >&2
    echo "       Convert the seed to ANSI prototype style before invoking the permuter." >&2
    echo "       (Your src/ file can remain K&R — only the permuter seed needs ANSI.)" >&2
    echo "" >&2
    exit 1
fi

# --- Derive function name + locate splat asm --------------------------------
# VRAM may have leading 0x; normalise to lowercase hex without prefix.
VRAM_HEX="$(printf '%X' "$((VRAM))")"  # uppercase, no prefix
FUNC_NAME="func_${VRAM_HEX}"
SIZE_DEC="$((SIZE))"

RUN_DIR="${RUNS_DIR}/${FUNC_NAME}"
mkdir -p "${RUN_DIR}"

# Find the asm file containing this function. Splat emits the function bodies
# into one .s file per top-level asm subsegment; the function we want lives in
# whichever one declares `glabel func_<VRAM>`.
ASM_SRC="$(grep -rl --include='*.s' -E "^glabel[[:space:]]+${FUNC_NAME}\b" "${PROJECT_ROOT}/asm" | head -1 || true)"
if [ -z "${ASM_SRC}" ]; then
    echo "ERROR: cannot locate splat asm for ${FUNC_NAME}. Did 'make split' run?" >&2
    exit 1
fi

# --- Build target.s (prelude + extracted function) --------------------------
TARGET_S="${RUN_DIR}/target.s"
{
    cat "${PERMUTER_DIR}/prelude.inc"
    echo
    # Extract from `glabel <FUNC>` to `endlabel <FUNC>` inclusive.
    awk -v fn="${FUNC_NAME}" '
        $0 ~ ("^glabel[[:space:]]+" fn "([[:space:]]|$)") { in_fn = 1 }
        in_fn { print }
        $0 ~ ("^endlabel[[:space:]]+" fn "([[:space:]]|$)") { in_fn = 0 }
    ' "${ASM_SRC}"
} > "${TARGET_S}"

# Sanity: confirm the extract has the expected size by counting the comment
# offsets between the first and last instruction.
INSN_COUNT="$(grep -cE '^[[:space:]]*/\*' "${TARGET_S}" || true)"
EXPECTED_INSNS="$(( SIZE_DEC / 4 ))"
if [ "${INSN_COUNT}" -ne "${EXPECTED_INSNS}" ]; then
    echo "WARNING: extracted ${INSN_COUNT} insns from asm but size ${SIZE} implies ${EXPECTED_INSNS}." >&2
fi

# Assemble target.s -> target.o (the permuter compares against this).
"${MIPS_PREFIX}as" -EB -march=vr4300 -mabi=32 -G 0 -no-pad-sections \
    -I"${PROJECT_ROOT}/include" \
    -o "${RUN_DIR}/target.o" "${TARGET_S}"

# --- Build base.c (preprocessed seed) ---------------------------------------
# Permuter expects a single-function .c file already run through cpp. Use the
# host cpp (preprocessor only); the IDO frontend will see this same file via
# compile.sh.
BASE_C="${RUN_DIR}/base.c"
cpp -P -undef -nostdinc \
    -D'__attribute__(x)=' \
    -D'_Static_assert(x, y)=' \
    -D'GLOBAL_ASM(...)=' \
    -I"${PROJECT_ROOT}/include" \
    "${SEED_C}" -o "${BASE_C}"

# Permuter wants exactly one function in base.c; strip_other_fns trims by name
# (rewrites the input file in place).
"${PYTHON}" "${PERMUTER_DIR}/strip_other_fns.py" "${BASE_C}" "${FUNC_NAME}"

# --- compile.sh — invoke IDO with the project's exact matching CFLAGS -------
COMPILE_SH="${RUN_DIR}/compile.sh"
cat > "${COMPILE_SH}" <<EOF
#!/usr/bin/env bash
set -e
INPUT="\$1"
OUTPUT=""
shift || true
while [ "\$#" -gt 0 ]; do
    case "\$1" in
        -o) OUTPUT="\$2"; shift 2 ;;
        *) shift ;;
    esac
done
if [ -z "\${OUTPUT}" ]; then
    echo "compile.sh: missing -o" >&2; exit 2
fi
"${IDO_CC}" -c -G 0 -non_shared -Xfullwarn -fullwarn -O2 -mips2 \\
    -woff 649,838,712,807 -signed -nostdinc \\
    -I"${PROJECT_ROOT}/include" \\
    -o "\${OUTPUT}" "\${INPUT}"
EOF
chmod +x "${COMPILE_SH}"

# --- settings.toml — keep it minimal, MIPS / IDO 7.1 ------------------------
cat > "${RUN_DIR}/settings.toml" <<'EOF'
compiler_type = "ido"
EOF

# --- Sanity: does base.c -> base.o match target.o on the unchanged seed? ----
# Run --debug for one iteration so the user can see the initial diff before
# the random-permutation loop kicks in.
echo "[permute_run.sh] run dir: ${RUN_DIR}"
echo "[permute_run.sh] func:    ${FUNC_NAME}  (size ${SIZE})"
echo "[permute_run.sh] target.s:    $(wc -l < "${TARGET_S}") lines"
echo "[permute_run.sh] base.c:      $(wc -l < "${BASE_C}") lines"

# --- Invoke permuter --------------------------------------------------------
# cd into the run dir so --debug's debug_source.c lands here instead of the
# project root.
LOG="${RUN_DIR}/permuter.log"
set +e
( cd "${RUN_DIR}" && \
    "${PYTHON}" "${PERMUTER_DIR}/permuter.py" \
        "${RUN_DIR}" \
        "${PERMUTER_EXTRA_ARGS[@]}" \
) 2>&1 | tee "${LOG}"
RC="${PIPESTATUS[0]}"
set -e

# Look for "found a match" in the log (permuter prints this when bytes match).
if grep -qE 'found a match' "${LOG}"; then
    echo
    echo "MATCH: found a permutation that matches ${FUNC_NAME}."
    # Permuter writes the matching .c into output/ inside the run dir.
    if [ -d "${RUN_DIR}/output" ]; then
        echo "Matching candidates: ${RUN_DIR}/output/"
        ls -1 "${RUN_DIR}/output" | head -10
    fi
    exit 0
fi

echo
echo "no permutation matched"
exit 1
