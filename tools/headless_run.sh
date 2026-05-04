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

CAP_SLEEP="${HEADLESS_CAP_SLEEP:-18000}"   # 5h conservative
COOL_SLEEP="${HEADLESS_COOL_SLEEP:-60}"
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

echo "$$" > "$PIDFILE"

cleanup() {
    log "received signal, exiting between iterations"
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

    # Capture stderr separately so we can pattern-match for cap hits.
    stderr_file=$(mktemp)
    set +e
    "$CLAUDE_BIN" --print "$(cat "$PROMPT_FILE")" \
        >> "$SHEPHERD_LOG" 2> "$stderr_file"
    rc=$?
    set -e
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
