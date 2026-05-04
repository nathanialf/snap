# Headless mode operator runbook

Unattended week-long matching loop. Single-host, single-shepherd,
shepherd-driven (not /loop). See `decomp/HEADLESS_PROMPT.md` for the
agent prompt and `tools/headless_run.sh` for the shepherd
implementation.

**Architecture: multi-match per session.** Each `claude --print`
invocation is a long-lived matching session that lands as many cluster
matches as it can, then exits cleanly when its 5h Claude usage cap is
about to hit. The shepherd waits for the cap window to reset, then
launches a fresh session. This amortizes the cold-start cost of
loading CLAUDE.md + saved memory across many matches per session
(Claude's auto-memory carries the IDO tricks, so the prompt does
**not** mandate re-reading those files at session start).

## Quick reference

```sh
# Launch
nohup ./tools/headless_run.sh > /dev/null 2>&1 & disown

# Stop (graceful — exits between iterations)
kill $(cat decomp/.headless.pid)

# Stop NOW (force-kill the in-flight claude; shepherd resumes next iter)
pkill -f 'claude --print'

# Stop hard (no grace, leaves nothing running)
kill -9 $(cat decomp/.headless.pid) 2>/dev/null; pkill -9 -f 'claude --print' 2>/dev/null; rm -f decomp/.headless.pid

# Status
cat decomp/.headless.pid && ps -p $(cat decomp/.headless.pid) -o pid,etime,cmd
tail -n 20 decomp/.headless_shepherd.log
tail -n 20 decomp/RUN_LOG.md
```

## Pre-flight

Before launching for the first time:

1. Confirm `claude` is on `$PATH` and authenticated:
   `claude --version` and `claude --print "echo ok"` both succeed.
2. Confirm green build at the target SHA-1: `make` exits MATCH.
3. Confirm tree is clean: `git status` empty.
4. Confirm IP-safety hook is installed (the shepherd does this on
   start, but you can pre-install): `tools/install_hooks.sh`.
5. Confirm no other matching session is active. Concurrent
   `make split` and concurrent `src/` edits *will* corrupt each other.

## Launch

```sh
cd /primary/dev/snap
nohup ./tools/headless_run.sh > /dev/null 2>&1 & disown
```

Verify it came up:

```sh
cat decomp/.headless.pid                       # live PID
tail -n 5 decomp/.headless_shepherd.log        # "started ... iter #1 starting"
```

## Monitor

```sh
tail -f decomp/RUN_LOG.md                      # per-iteration outcomes
tail -f decomp/.headless_shepherd.log          # shepherd lifecycle + heartbeats
ls -1t decomp/.headless_debug/                 # per-iter verbose debug logs
tail -f decomp/.headless_debug/iter_*.debug.log | head -1  # current iter live
git log --since="24 hours ago" --oneline       # committed progress
make progress                                  # refresh percentage tables on demand
```

The shepherd emits a heartbeat line to `.headless_shepherd.log` every
30 seconds while a `claude --print` is in flight, of the form:

```
2026-05-04T03:01:09+00:00 shepherd: iter #2 alive @60s | <last debug line truncated to 200 chars>
```

The "last debug line" is from `decomp/.headless_debug/iter_NNNNN.debug.log`,
which captures `claude -d "tool" --debug-file=…` output — i.e. the
agent's tool calls as they happen. The shepherd keeps the last
`HEADLESS_DEBUG_KEEP` (default 10) iterations' debug logs and rotates
older ones at iteration start.

`decomp/RUN_LOG.md` and `decomp/.headless_debug/` are gitignored — they
live only on this host.

## Stop

Graceful (waits for the in-flight iteration to finish):

```sh
kill $(cat decomp/.headless.pid)
```

`SIGTERM` is trapped; the shepherd exits between iterations and
removes its PID file. Force-stop a stuck `claude --print` (the
shepherd will then sleep and resume next iteration):

```sh
pkill -f 'claude --print'
```

Hard kill the whole loop (no grace):

```sh
kill -9 $(cat decomp/.headless.pid); rm -f decomp/.headless.pid
```

## Health checks (run any time)

- `git status` clean → no orphan from a crashed iteration.
- `make` exits MATCH → build still green at the target SHA-1.
- `tools/check_no_rom.sh` exits 0 → IP-clean.
- `ls -l .git/hooks/pre-commit` → IP-safety hook still installed.

If any of these fail, pause the shepherd before investigating: the
next iteration's session-start sanity check usually self-heals, but
manual intervention is safer if the failure is unexpected.

## Disk hygiene

`tools/decomp-permuter/runs/output-*` accumulates. Manual cleanup
recommended weekly (do **not** run while the shepherd is mid-iteration —
the permuter may have a live `runs/<func>/` directory):

```sh
# stop shepherd first
kill $(cat decomp/.headless.pid)
# wait for log to confirm "exiting between iterations"
find tools/decomp-permuter/runs -maxdepth 2 -name 'output-*' -mtime +3 -exec rm -rf {} +
# relaunch
nohup ./tools/headless_run.sh > /dev/null 2>&1 & disown
```

## Tunables (env vars)

- `HEADLESS_CAP_SLEEP` — seconds to sleep after a detected cap hit.
  Default: `18000` (5h). Lower if the cap window is shorter than 5h.
- `HEADLESS_COOL_SLEEP` — seconds between iterations on a normal
  exit. Default: `60`. Lower for max throughput, higher to be a
  better filesystem/git citizen.
- `HEADLESS_CLAUDE` — path to the `claude` binary. Default: `claude`
  via `$PATH`.
- `HEADLESS_HEARTBEAT` — seconds between heartbeat lines while a
  `claude --print` is in flight. Default: `30`. Set to `0` to disable
  (not recommended — silence is indistinguishable from hang).
- `HEADLESS_DEBUG_KEEP` — number of recent per-iter debug logs to
  retain under `decomp/.headless_debug/`. Default: `10`.

## Tough-nut handling

Iterations come in two flavours. The agent decides per-iteration which
to run, based on signals it reads from RUN_LOG.md and recent commits:

- **Fresh iteration**: pick a small unmatched cluster from
  `find_siblings.py`, attempt match, 5-min permuter budget. Deferred
  entries from `MATCHING_NOTES.md` are skipped on this path.
- **Revisit iteration**: pick a *deferred* entry from
  `MATCHING_NOTES.md`, apply the leverage playbook (cross-pattern
  reuse, multiple source-shape variations, 30-min permuter budget),
  update plateau / hint info even on a non-zero permuter exit so the
  *next* revisit is cheaper.

Cadence: at least one revisit per ~6 productive iterations, plus
opportunistic revisits when a recent commit added new leverage (saved
memory, NOTES.md pattern, sibling family crack). RUN_LOG.md rows tag
revisit iterations as `revisit` so cadence is visible at a glance.

If you want to manually nudge the loop toward a specific deferred
entry, edit its line in `docs/MATCHING_NOTES.md` to add a fresh
"recent leverage" hint — the agent reads MATCHING_NOTES on each
revisit and is more likely to pick the freshest entry.

## Known limits

- **No PC-restart survival.** Shepherd is `nohup`-backgrounded; a
  host reboot kills it. Re-launch manually after reboot.
- **No multi-host coordination.** One shepherd per checkout. The
  PID-file check refuses a second shepherd on the same host but does
  not coordinate across hosts.
- **Cap detection is heuristic.** Short non-zero `claude` exits with
  "usage limit"/"rate limit" in stderr trigger the long sleep; other
  failure shapes get the short cool-off. False positives waste up to
  5h; false negatives loop tightly into the cap and end up sleeping
  on a successful (zero-progress) iteration.
- **Run log is local-only.** `decomp/RUN_LOG.md` is gitignored. To
  audit a week's run remotely, use `git log` (per-match commits).
