#!/usr/bin/env bash
# =============================================================================
# headless_run.sh — shepherd for the unattended Pokémon Snap matching loop.
#
# Launch:  nohup ./tools/headless_run.sh > /dev/null 2>&1 & disown
# Stop:    kill $(cat decomp/.headless.pid)   (TERM is trapped between iters)
#
# Each iteration spawns a fresh `claude --print` reading
# decomp/HEADLESS_PROMPT.md as its prompt. The agent does one match-attempt
# cycle, commits or defers, appends a row to decomp/RUN_LOG.md, and exits.
# Per-match commits are gated by the IP-safety pre-commit hook.
#
# This script is the scheduler — it is NOT itself a /loop or ScheduleWakeup
# user. See decomp/HEADLESS.md for the operator runbook.
# =============================================================================
set -uo pipefail

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PROMPT_FILE="${PROJECT_ROOT}/decomp/HEADLESS_PROMPT.md"
RUN_LOG="${PROJECT_ROOT}/decomp/RUN_LOG.md"
SHEPHERD_LOG="${PROJECT_ROOT}/decomp/.headless_shepherd.log"
PIDFILE="${PROJECT_ROOT}/decomp/.headless.pid"
INSTALL_HOOKS="${PROJECT_ROOT}/tools/install_hooks.sh"
DEBUG_DIR="${PROJECT_ROOT}/decomp/.headless_debug"

CAP_SLEEP="${HEADLESS_CAP_SLEEP:-18000}"   # 5h conservative
COOL_SLEEP="${HEADLESS_COOL_SLEEP:-60}"
HEARTBEAT_SEC="${HEADLESS_HEARTBEAT:-30}"
DEBUG_KEEP="${HEADLESS_DEBUG_KEEP:-10}"
CLAUDE_BIN="${HEADLESS_CLAUDE:-claude}"

ts()  { date -Iseconds; }
log() { printf '%s shepherd: %s\n' "$(ts)" "$*" >> "$SHEPHERD_LOG"; }

if [[ ! -r "$PROMPT_FILE" ]]; then
    echo "ERROR: prompt file missing: $PROMPT_FILE" >&2
    exit 2
fi

# Refuse to start if another shepherd is alive.
if [[ -f "$PIDFILE" ]]; then
    other=$(cat "$PIDFILE" 2>/dev/null || true)
    if [[ -n "$other" ]] && kill -0 "$other" 2>/dev/null; then
        echo "ERROR: another shepherd appears alive (pid $other in $PIDFILE)" >&2
        exit 3
    fi
    rm -f "$PIDFILE"
fi

# Idempotent re-install of the IP-safety pre-commit hook.
if [[ -x "$INSTALL_HOOKS" ]]; then
    "$INSTALL_HOOKS" >/dev/null
fi

mkdir -p "$DEBUG_DIR"
echo "$$" > "$PIDFILE"

# Track the in-flight claude PID + heartbeat PID so the trap can kill them.
CLAUDE_PID=""
HEARTBEAT_PID=""

cleanup() {
    log "received signal, exiting between iterations"
    [[ -n "$HEARTBEAT_PID" ]] && kill "$HEARTBEAT_PID" 2>/dev/null || true
    [[ -n "$CLAUDE_PID" ]]    && kill "$CLAUDE_PID"    2>/dev/null || true
    rm -f "$PIDFILE"
    exit 0
}
trap cleanup TERM INT

log "started (pid $$, prompt $PROMPT_FILE)"

iter=0
while true; do
    iter=$((iter + 1))
    log "iter #${iter} starting"
    start=$(date +%s)

    debug_file="${DEBUG_DIR}/iter_$(printf '%05d' "$iter").debug.log"
    : > "$debug_file"

    # Rotate: keep only the last $DEBUG_KEEP debug logs.
    ls -1t "$DEBUG_DIR"/iter_*.debug.log 2>/dev/null \
        | tail -n +$((DEBUG_KEEP + 1)) | xargs -r rm -f

    # Backgrounded heartbeat: emit "alive" + last debug line every $HEARTBEAT_SEC.
    (
        hb_iter=$iter
        hb_start=$start
        while sleep "$HEARTBEAT_SEC"; do
            now=$(date +%s)
            dur=$((now - hb_start))
            last_debug="$(tail -n 1 "$debug_file" 2>/dev/null | tr -d '\r' | head -c 200)"
            if [[ -z "$last_debug" ]]; then last_debug="(no debug output yet)"; fi
            printf '%s shepherd: iter #%d alive @%ds | %s\n' \
                "$(ts)" "$hb_iter" "$dur" "$last_debug" >> "$SHEPHERD_LOG"
        done
    ) &
    HEARTBEAT_PID=$!

    # Capture stderr separately so we can pattern-match for cap hits.
    stderr_file=$(mktemp)
    set +e
    "$CLAUDE_BIN" --print --debug "tool" --debug-file="$debug_file" \
        "$(cat "$PROMPT_FILE")" \
        >> "$SHEPHERD_LOG" 2> "$stderr_file" &
    CLAUDE_PID=$!
    wait "$CLAUDE_PID"
    rc=$?
    set -e

    # Stop heartbeat now that claude has exited.
    kill "$HEARTBEAT_PID" 2>/dev/null || true
    wait "$HEARTBEAT_PID" 2>/dev/null || true
    HEARTBEAT_PID=""
    CLAUDE_PID=""

    cat "$stderr_file" >> "$SHEPHERD_LOG"
    end=$(date +%s)
    dur=$((end - start))

    cap_hit=0
    if (( rc != 0 )) && (( dur < 60 )); then
        if grep -qiE 'usage limit|rate.?limit|usage cap|too many requests' "$stderr_file"; then
            cap_hit=1
        fi
    fi
    rm -f "$stderr_file"

    log "iter #${iter} exited rc=${rc} dur=${dur}s cap_hit=${cap_hit}"

    if (( cap_hit == 1 )); then
        log "cap hit: sleeping ${CAP_SLEEP}s"
        sleep "$CAP_SLEEP"
    else
        sleep "$COOL_SLEEP"
    fi
done
