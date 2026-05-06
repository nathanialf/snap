#!/usr/bin/env bash
# auto_permute_tough_nuts.sh — round-robin permuter over tough_nuts/.
#
# Iterates each tough_nuts/<func_name>/ that has a matching <func_name>.c seed,
# resolves VRAM and size, and runs tools/permute_run.sh against it. The
# per-function permuter is invoked with --stop-on-zero --best-only so the
# log captures only score-improvements + the final match (if any) — no
# noise from "iter 12345, score 200" tickers.
#
# **Progress IS preserved across runs**, by promoting the best-scoring
# candidate from each run back into the tough_nuts seed file. Mechanism:
# permuter writes improved candidates to runs/<func>/output/source_N.c
# where N is the score (lower = better). After each per-function run,
# this script picks the lowest-N source file and overwrites
# tough_nuts/<func>/<func>.c with it. The next run starts from that
# improved seed instead of the original. (Original seeds are preserved in
# git history — `git diff tough_nuts/<func>/` shows the cumulative
# improvement.)
#
# On a match, the matching candidate (decomp-permuter writes it to
# tools/decomp-permuter/runs/<func>/output/) is left in place; this script
# does NOT auto-promote into src/ or flip the yaml — that's a manual review
# step (because the tough_nut notes.md may need updating, the matching .c may
# need de-uglifying before commit, and the operator may want to inspect the
# permutation before landing it). Instead, the script prints a clear
# "MATCH: <func>" line and moves on.
#
# Usage:
#   tools/auto_permute_tough_nuts.sh          # round-robin all tough_nuts
#   tools/auto_permute_tough_nuts.sh func_X   # only this one
#
# **No timeouts.** A function only "advances" in the round-robin when the
# permuter exits — either because it found a match (--stop-on-zero) or
# because the user Ctrl-C'd it out of a plateau. The log captures every
# improvement (--best-only), so leave it running passively and check the
# activity log periodically: `tail -f tools/decomp-permuter/auto_tough_nuts.log`.
#
# Env:
#   ITERATIONS=N      cap total round-robin passes (default: infinite)
#   THREADS=N         pass -j N to permuter (default: leave unset)
#   SKIP_MATCHED=1    skip subdirs whose function is already matched
#                      (default: 1, set to 0 to retry matched ones)
#   STOP_AT_SCORE=N   advance to next function once permuter has produced
#                      an output with score <= N (default: 0 = match-only).
#                      A score-50 result is hand-attackable: the wrapper
#                      will kill permuter and move on so the operator can
#                      review/promote the result manually.
#
# Output:
#   Logs from each per-function run land in
#   tools/decomp-permuter/runs/<func>/permuter.log (overwritten per run).
#   This wrapper writes a top-level activity log to
#   tools/decomp-permuter/auto_tough_nuts.log

set -uo pipefail

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TOUGH_DIR="${PROJECT_ROOT}/tough_nuts"
PERMUTE_RUN="${PROJECT_ROOT}/tools/permute_run.sh"
ACTIVITY_LOG="${PROJECT_ROOT}/tools/decomp-permuter/auto_tough_nuts.log"
SKIP_MATCHED="${SKIP_MATCHED:-1}"
ITERATIONS="${ITERATIONS:-0}"   # 0 = infinite
THREADS="${THREADS:-}"
STOP_AT_SCORE="${STOP_AT_SCORE:-0}"  # 0 = match-only; >0 = advance once score <= N

if [ ! -d "${TOUGH_DIR}" ]; then
    echo "ERROR: tough_nuts/ directory not found at ${TOUGH_DIR}" >&2
    exit 1
fi
if [ ! -x "${PERMUTE_RUN}" ]; then
    echo "ERROR: permute_run.sh not executable at ${PERMUTE_RUN}" >&2
    exit 1
fi

mkdir -p "$(dirname "${ACTIVITY_LOG}")"

log() {
    local msg="$*"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] ${msg}" | tee -a "${ACTIVITY_LOG}"
}

# On SIGINT (operator Ctrl-C): promote the in-flight function's best
# candidate into its seed before exiting, so progress isn't lost.
CURRENT_NAME=""
CURRENT_SEED=""
on_interrupt() {
    log "interrupt received; promoting in-flight progress before exit"
    if [ -n "${CURRENT_NAME}" ] && [ -n "${CURRENT_SEED}" ]; then
        promote_best_into_seed "${CURRENT_NAME}" "${CURRENT_SEED}"
    fi
    exit 130
}
trap on_interrupt INT TERM

# Resolve VRAM from func_<8 hex digits> name.
vram_of() {
    local name="$1"
    if [[ "${name}" =~ ^func_([0-9A-Fa-f]{8})$ ]]; then
        printf '0x%s\n' "${BASH_REMATCH[1]}"
    else
        return 1
    fi
}

# Find the function's size by searching all asm/*.s files for
# "nonmatching <funcname>, <size>" entries left by spimdisasm.
size_of() {
    local name="$1"
    local hit
    hit="$(grep -rE "^nonmatching[[:space:]]+${name},[[:space:]]+0x[0-9A-Fa-f]+" \
        "${PROJECT_ROOT}/asm/" 2>/dev/null | head -1)"
    if [ -z "${hit}" ]; then
        return 1
    fi
    echo "${hit}" | sed -nE 's/.*0x([0-9A-Fa-f]+).*/0x\1/p'
}

# Skip if the function is already wired into the build (i.e. operator
# promoted a match into src/), OR the permuter has already produced a
# matching candidate for it (output/ dir exists in the per-function run dir).
already_done() {
    local name="$1"
    # Wired into the build?
    if [ -f "${PROJECT_ROOT}/src/${name}.c" ]; then
        return 0
    fi
    if grep -lE "^[a-z][a-z0-9 *_]* ${name}[(]" "${PROJECT_ROOT}/src/"*.c >/dev/null 2>&1; then
        return 0
    fi
    # Permuter already found a score-0 match in a prior run?
    local run_dir="${PROJECT_ROOT}/tools/decomp-permuter/runs/${name}"
    if ls -d "${run_dir}"/output-0-* >/dev/null 2>&1; then
        return 0
    fi
    return 1
}

# Build the list of candidate subdirs.
collect_candidates() {
    local arg="${1:-}"
    if [ -n "${arg}" ]; then
        if [ -d "${TOUGH_DIR}/${arg}" ]; then
            echo "${arg}"
        else
            echo "ERROR: tough_nuts/${arg}/ not found" >&2
            exit 1
        fi
        return
    fi
    for dir in "${TOUGH_DIR}"/*/; do
        local name
        name="$(basename "${dir}")"
        # Must have a matching seed .c file
        [ -f "${dir}${name}.c" ] || continue
        # Skip already-done (matched in src/ OR permuter already produced output)
        if [ "${SKIP_MATCHED}" = "1" ] && already_done "${name}"; then
            continue
        fi
        echo "${name}"
    done
}

run_one() {
    local name="$1"
    local seed="${TOUGH_DIR}/${name}/${name}.c"
    local vram size
    vram="$(vram_of "${name}")" || { log "skip ${name}: cannot derive VRAM"; return 1; }
    size="$(size_of "${name}")" || { log "skip ${name}: cannot derive size"; return 1; }

    log "=== START ${name} (vram=${vram}, size=${size})"
    # Build permuter extra-args:
    #   --stop-on-zero  → permuter exits when score 0 found, advancing the loop
    #   --best-only     → suppress noisy iter tickers; only log improvements
    #   -j THREADS      → optional parallelism
    local extra=(-- --stop-on-zero --best-only)
    if [ -n "${THREADS}" ]; then
        extra+=(-j "${THREADS}")
    fi
    local rc=0
    # CURRENT_NAME / CURRENT_SEED are read by the SIGINT trap so a user
    # Ctrl-C still promotes whatever progress the permuter made before
    # propagating the interrupt and exiting.
    CURRENT_NAME="${name}"; CURRENT_SEED="${seed}"

    # Optional STOP_AT_SCORE watchdog: poll runs/<name>/ for output-N-* dirs;
    # once any with N <= STOP_AT_SCORE appears, kill the permuter and advance.
    local watchdog_pid=""
    local out_dir="${PROJECT_ROOT}/tools/decomp-permuter/runs/${name}"
    if [ "${STOP_AT_SCORE}" -gt 0 ]; then
        (
            while sleep 5; do
                if ls -d "${out_dir}"/output-*-* 2>/dev/null \
                    | sed -nE 's|.*/output-([0-9]+)-.*|\1|p' \
                    | sort -n | head -1 \
                    | awk -v t="${STOP_AT_SCORE}" '{ exit !($1+0 <= t) }'; then
                    log "  STOP_AT_SCORE: ${name} reached score <= ${STOP_AT_SCORE}, killing permuter"
                    pkill -P "$$" -f "permute_run.sh" 2>/dev/null || true
                    pkill -f "permuter.py.*${name}" 2>/dev/null || true
                    exit 0
                fi
            done
        ) &
        watchdog_pid=$!
    fi

    "${PERMUTE_RUN}" "${vram}" "${size}" "${seed}" "${extra[@]}" \
        >>"${ACTIVITY_LOG}" 2>&1 || rc=$?

    if [ -n "${watchdog_pid}" ]; then
        kill "${watchdog_pid}" 2>/dev/null || true
        wait "${watchdog_pid}" 2>/dev/null || true
    fi
    # rc==0 → match found. rc!=0 → no match yet (permuter exited naturally).
    # Either way, promote the best-scoring candidate into the seed for the
    # next pass so we don't re-do work.
    promote_best_into_seed "${name}" "${seed}"
    CURRENT_NAME=""; CURRENT_SEED=""
    if [ "${rc}" -eq 0 ]; then
        log "MATCH ${name}"
        return 0
    fi

    # If STOP_AT_SCORE is set, check whether we reached threshold and log it.
    if [ "${STOP_AT_SCORE}" -gt 0 ] && [ -d "${out_dir}" ]; then
        local best
        best=$(ls -d "${out_dir}"/output-*-* 2>/dev/null \
            | sed -nE 's|.*/output-([0-9]+)-.*|\1|p' \
            | sort -n | head -1)
        if [ -n "${best}" ] && [ "${best}" -le "${STOP_AT_SCORE}" ]; then
            log "PLATEAU ${name}  (best score=${best}, advancing per STOP_AT_SCORE=${STOP_AT_SCORE})"
            return 0
        fi
    fi

    log "no-match ${name}  (Ctrl-C'd or permuter exited; seed updated to best so far)"
    return 1
}

# Promote the lowest-score candidate from runs/<name>/output-N-K/source.c
# into the tough_nuts seed file, so the next round-robin pass starts from
# improved C instead of the original seed. Permuter writes:
#   runs/<name>/output-<score>-<num>/source.c
# where score is the number we want to minimize (0 = perfect match).
promote_best_into_seed() {
    local name="$1"
    local seed="$2"
    local run_dir="${PROJECT_ROOT}/tools/decomp-permuter/runs/${name}"
    [ -d "${run_dir}" ] || return 0
    # Find the output-N-K directory with the smallest N.
    local best_dir
    best_dir="$(ls -d "${run_dir}"/output-*-* 2>/dev/null | \
        sed -nE 's|.*/output-([0-9]+)-.*|\1 &|p' | \
        sort -n | head -1 | awk '{print $2}')"
    if [ -z "${best_dir}" ] || [ ! -f "${best_dir}/source.c" ]; then
        return 0
    fi
    local best_path="${best_dir}/source.c"
    local best_score
    best_score="$(basename "${best_dir}" | sed -nE 's|^output-([0-9]+)-.*|\1|p')"
    # Compare to current seed — only promote if different (avoid noisy diffs).
    if cmp -s "${best_path}" "${seed}"; then
        log "  best already matches seed (score ${best_score}, no promotion)"
        return 0
    fi
    cp "${best_path}" "${seed}"
    log "  promoted best score=${best_score} into ${seed#${PROJECT_ROOT}/}"
}

# Re-read candidates each pass so newly-matched ones drop out of rotation.
PASS=0
EXPLICIT_ARG="${1:-}"
while :; do
    PASS=$((PASS + 1))
    mapfile -t CANDIDATES < <(collect_candidates "${EXPLICIT_ARG}")
    if [ "${#CANDIDATES[@]}" -eq 0 ]; then
        log "no remaining candidates in ${TOUGH_DIR} — exiting."
        break
    fi
    log "=== PASS ${PASS} (${#CANDIDATES[@]} remaining) ==="
    for c in "${CANDIDATES[@]}"; do log "  - ${c}"; done
    for name in "${CANDIDATES[@]}"; do
        # Re-check "already done" each iteration too — a match in a prior
        # tough_nut's run may have promoted into src/, marking this one done.
        if [ "${SKIP_MATCHED}" = "1" ] && already_done "${name}"; then
            log "skip ${name} (already done)"
            continue
        fi
        run_one "${name}" || true   # don't exit on no-match
    done
    if [ "${ITERATIONS}" -gt 0 ] && [ "${PASS}" -ge "${ITERATIONS}" ]; then
        log "completed ${ITERATIONS} pass(es), exiting."
        break
    fi
done
