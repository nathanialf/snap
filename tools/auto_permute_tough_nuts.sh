#!/usr/bin/env bash
# auto_permute_tough_nuts.sh — parallel permuter pool over tough_nuts/.
#
# Iterates each tough_nuts/<func_name>/ that has a matching <func_name>.c seed,
# resolves VRAM and size, and runs tools/permute_run.sh against it. Up to
# PARALLEL functions are permuted concurrently (default 4), each pinned to
# `-j 1` so the total concurrent thread count equals PARALLEL — i.e. each
# core handles its own function.
#
# **Progress IS preserved across runs**, by promoting the best-scoring
# candidate from each run back into the tough_nuts seed file. Mechanism:
# permuter writes improved candidates to runs/<func>/output-<score>-<n>/source.c
# (lower score = better). After each per-function run, this script picks
# the lowest-score source.c and overwrites tough_nuts/<func>/<func>.c with
# it. The next run starts from that improved seed instead of the original.
# (Original seeds are preserved in git history — `git diff tough_nuts/<func>/`
# shows the cumulative improvement.)
#
# On a match, the matching candidate is left in place; this script does
# NOT auto-promote into src/ or flip the yaml — that's a manual review
# step. Instead, the script prints a clear "MATCH: <func>" line and
# moves on.
#
# Usage:
#   tools/auto_permute_tough_nuts.sh          # parallel pool over all tough_nuts
#   tools/auto_permute_tough_nuts.sh func_X   # only this one (single, not parallel)
#
# Env:
#   PARALLEL=N        max concurrent permuter processes (default: 4).
#                      Each uses `-j 1`, so total cores ≈ PARALLEL.
#   ITERATIONS=N      cap total round-robin passes (default: infinite)
#   SKIP_MATCHED=1    skip subdirs whose function is already matched
#                      (default: 1, set to 0 to retry matched ones)
#   STOP_AT_SCORE=N   advance to next function once permuter has produced
#                      an output with score <= N (default: 0 = match-only).
#                      A score-50 result is hand-attackable: the wrapper
#                      will kill that function's permuter and move on so
#                      the operator can review/promote the result manually.
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
PARALLEL="${PARALLEL:-4}"
STOP_AT_SCORE="${STOP_AT_SCORE:-0}"

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

# Track active worker PIDs → function names so we can promote/cleanup
# the right seed when a worker finishes.
declare -A WORKER_NAME=()   # pid → name

# On SIGINT/TERM: kill all workers, promote whatever they had.
on_interrupt() {
    log "interrupt received; killing workers and promoting in-flight progress"
    for pid in "${!WORKER_NAME[@]}"; do
        local name="${WORKER_NAME[$pid]}"
        kill -TERM "${pid}" 2>/dev/null || true
        # Reap and promote
        wait "${pid}" 2>/dev/null || true
        promote_best_into_seed "${name}" "${TOUGH_DIR}/${name}/${name}.c"
    done
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
# matching candidate for it (output-0-* dir exists).
already_done() {
    local name="$1"
    if [ -f "${PROJECT_ROOT}/src/${name}.c" ]; then
        return 0
    fi
    if grep -lE "^[a-z][a-z0-9 *_]* ${name}[(]" "${PROJECT_ROOT}/src/"*.c >/dev/null 2>&1; then
        return 0
    fi
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
        [ -f "${dir}${name}.c" ] || continue
        if [ "${SKIP_MATCHED}" = "1" ] && already_done "${name}"; then
            continue
        fi
        echo "${name}"
    done
}

# Run permute_run.sh for one function in a subshell (so we get a single
# parent PID we can wait on / kill independently). Sets up its own
# STOP_AT_SCORE watchdog targeting only its own PID, not any siblings.
worker() {
    local name="$1"
    local seed="${TOUGH_DIR}/${name}/${name}.c"
    local vram size
    vram="$(vram_of "${name}")" || { log "skip ${name}: cannot derive VRAM"; return 1; }
    size="$(size_of "${name}")" || { log "skip ${name}: cannot derive size"; return 1; }

    log "  START ${name} (vram=${vram}, size=${size})"
    # Each worker gets exactly 1 permuter thread so PARALLEL workers
    # consume PARALLEL cores total.
    local extra=(-- --stop-on-zero --best-only -j 1)
    local rc=0
    local out_dir="${PROJECT_ROOT}/tools/decomp-permuter/runs/${name}"

    # Run permuter in a sub-process so we can hold its PID for the
    # STOP_AT_SCORE watchdog to target without nuking siblings.
    "${PERMUTE_RUN}" "${vram}" "${size}" "${seed}" "${extra[@]}" \
        >>"${ACTIVITY_LOG}" 2>&1 &
    local permuter_pid=$!

    # Optional STOP_AT_SCORE watchdog: poll runs/<name>/ for output-N-* dirs;
    # once any with N <= STOP_AT_SCORE appears, kill THIS permuter only.
    local watchdog_pid=""
    if [ "${STOP_AT_SCORE}" -gt 0 ]; then
        (
            while kill -0 "${permuter_pid}" 2>/dev/null; do
                sleep 5
                if ls -d "${out_dir}"/output-*-* 2>/dev/null \
                    | sed -nE 's|.*/output-([0-9]+)-.*|\1|p' \
                    | sort -n | head -1 \
                    | awk -v t="${STOP_AT_SCORE}" '{ exit !($1+0 <= t) }'; then
                    log "    STOP_AT_SCORE: ${name} reached score <= ${STOP_AT_SCORE}, killing permuter"
                    # Kill the whole process tree rooted at permuter_pid.
                    pkill -TERM -P "${permuter_pid}" 2>/dev/null || true
                    kill -TERM "${permuter_pid}" 2>/dev/null || true
                    break
                fi
            done
        ) &
        watchdog_pid=$!
    fi

    wait "${permuter_pid}" || rc=$?
    if [ -n "${watchdog_pid}" ]; then
        kill "${watchdog_pid}" 2>/dev/null || true
        wait "${watchdog_pid}" 2>/dev/null || true
    fi

    promote_best_into_seed "${name}" "${seed}"

    if [ "${rc}" -eq 0 ]; then
        log "MATCH ${name}"
        return 0
    fi

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

    log "  no-match ${name}  (permuter exited; seed updated to best so far)"
    return 1
}

# Promote the lowest-score candidate from runs/<name>/output-N-K/source.c
# into the tough_nuts seed file. Permuter writes:
#   runs/<name>/output-<score>-<num>/source.c
# where score is the number we want to minimize (0 = perfect match).
promote_best_into_seed() {
    local name="$1"
    local seed="$2"
    local run_dir="${PROJECT_ROOT}/tools/decomp-permuter/runs/${name}"
    [ -d "${run_dir}" ] || return 0
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
    if cmp -s "${best_path}" "${seed}"; then
        log "  best already matches seed (score ${best_score}, no promotion)"
        return 0
    fi
    cp "${best_path}" "${seed}"
    log "  promoted best score=${best_score} into ${seed#${PROJECT_ROOT}/}"
}

# Reap any finished workers; remove them from WORKER_NAME so the pool
# can refill. Uses non-blocking `wait -p` (bash 5.1+) via a kill -0
# probe instead so older bash works.
reap_finished() {
    local pid
    for pid in "${!WORKER_NAME[@]}"; do
        if ! kill -0 "${pid}" 2>/dev/null; then
            wait "${pid}" 2>/dev/null || true
            unset 'WORKER_NAME[$pid]'
        fi
    done
}

# Main: parallel pool over candidates.
PASS=0
EXPLICIT_ARG="${1:-}"

# Single-function mode: just run it directly (no pool).
if [ -n "${EXPLICIT_ARG}" ]; then
    worker "${EXPLICIT_ARG}" || true
    exit 0
fi

while :; do
    PASS=$((PASS + 1))
    mapfile -t CANDIDATES < <(collect_candidates)
    if [ "${#CANDIDATES[@]}" -eq 0 ]; then
        # Drain any in-flight workers, then exit.
        for pid in "${!WORKER_NAME[@]}"; do wait "${pid}" 2>/dev/null || true; done
        log "no remaining candidates in ${TOUGH_DIR} — exiting."
        break
    fi
    log "=== PASS ${PASS} (${#CANDIDATES[@]} remaining, parallel=${PARALLEL}) ==="

    for name in "${CANDIDATES[@]}"; do
        # Wait for a slot to free up before starting next.
        while [ "${#WORKER_NAME[@]}" -ge "${PARALLEL}" ]; do
            wait -n 2>/dev/null || sleep 2
            reap_finished
        done

        # Re-check "already done" — a sibling worker may have just promoted
        # into src/ between when we built CANDIDATES and now.
        if [ "${SKIP_MATCHED}" = "1" ] && already_done "${name}"; then
            log "skip ${name} (already done)"
            continue
        fi

        worker "${name}" &
        local_pid=$!
        WORKER_NAME[${local_pid}]="${name}"
        log "  spawned ${name} as pid=${local_pid} (active=${#WORKER_NAME[@]}/${PARALLEL})"
    done

    # Drain the pool before declaring the pass done.
    while [ "${#WORKER_NAME[@]}" -gt 0 ]; do
        wait -n 2>/dev/null || sleep 2
        reap_finished
    done

    if [ "${ITERATIONS}" -gt 0 ] && [ "${PASS}" -ge "${ITERATIONS}" ]; then
        log "completed ${ITERATIONS} pass(es), exiting."
        break
    fi
done
