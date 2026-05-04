# Headless single-iteration prompt

This file is the prompt fed to each `claude --print` invocation by
`tools/headless_run.sh`. **You are a single iteration of an unattended
matching loop.** Do exactly one match-attempt cycle, then exit cleanly.

**Hard rules:**

- Do not start `/loop`. Do not call `ScheduleWakeup` or `CronCreate`.
- Do not ask the user questions — there is no user attached.
- Do not push to any remote. Do not bypass git hooks (`--no-verify`).
- Do not add an AI co-author trailer.
- Respect the IP rules in `CLAUDE.md`. The `tools/check_no_rom.sh`
  pre-commit hook will block bad commits — do not work around it; if it
  blocks, treat the iteration as a recovery iteration (revert and exit).
- Do not modify `tools/headless_run.sh` or this file
  (`decomp/HEADLESS_PROMPT.md`) from inside an iteration.

## 1. Read first (in order)

1. `CLAUDE.md` — IP rules, build commands, exception scope.
2. `decomp/NOTES.md` — IDO/splat/linker gotchas.
3. `docs/MATCHING_NOTES.md` — tough-nut entries and deferred targets.
4. `decomp/STARTING_PROMPT.md` — the matching workflow itself
   (toolkit, per-cluster steps, leverage strategy). The "Wrap-up" rules
   apply to you too.
5. `git log -10 --oneline` and `git status` — state from the previous
   iteration.

## 2. Session-start sanity check (recovery from a crashed iteration)

Eager-commit means a clean tree is the normal entry state. If `git
status` is **dirty**, the previous iteration crashed mid-flow. Recover:

- Run `make`. If exit is **MATCH**: stage `src/`, `config/`,
  `decomp/`, `docs/`, and any `tools/` edits, then commit (the previous
  iteration matched but died before committing). Append a `recovered`
  row to `decomp/RUN_LOG.md` and exit. **This iteration is done.**
- If exit is **MISMATCH** or build-red:
  `git restore config/snap.us.yaml`, `rm src/<orphaned>.c`,
  `make split && make` to confirm green. Append a `recovered` row to
  RUN_LOG.md and exit. **This iteration is done.**

Recovery iterations do not pick a new target. They exit clean.

## 3. Target selection

Clean tree → pick exactly one target. Decide first whether this is a
**fresh** iteration or a **revisit** iteration:

### Revisit iteration (every ~6th iteration, or on signal)

Tough nuts won't crack themselves. Trigger a revisit iteration when
**any** of:

- The last 5 RUN_LOG.md rows are all `matched` or `consolidated` with
  no `revisit` entry in between. (Cap: at least one revisit per 6
  productive iterations.)
- A recent commit (within the last ~10) added a new saved-feedback
  memory under `~/.claude/projects/-primary-dev-snap/memory/` or a
  new "IDO codegen patterns to recognise" entry in `decomp/NOTES.md`.
  New leverage = retry deferred entries that might be in scope.
- A recent match cracked a sibling family (commit message contains
  "match" + a function near a deferred entry's address).

On a revisit iteration:

1. Read `docs/MATCHING_NOTES.md` deferred / tough-nut sections in
   full. Pick **one** entry whose failure mode plausibly matches a
   recently-added trick (saved memory, NOTES.md pattern, sibling
   family). Prefer entries with the lowest recorded permuter plateau
   score — those are closest to cracking.
2. Apply the leverage playbook from `STARTING_PROMPT.md` "Building
   leverage on tough nuts": cross-pattern reuse first, source-shape
   variations second, permuter third.
3. Try **multiple** source-shape variations (3–5) before falling back
   to permuter. Saved-memory tricks: K&R sub-variants, volatile
   pointer trick, struct-by-value, `mul.s` operand swap, bnel
   do-while, arg^0 spill — work through each that could plausibly fit.
4. If a permuter run is needed, use the **revisit budget**:
   `timeout 1800 tools/permute_run.sh <vram> <size> <seed>` — 30 min
   wall-clock cap. Inspect the best-candidate `source.c` even on
   non-zero exit and update the deferred entry's notes with new
   plateau / hint info before exiting.
5. **Outcome rows in RUN_LOG.md must use `revisit` as the iter type**
   (not `iter`) — see §6.

A revisit that doesn't crack still has value: an updated plateau
score and a new structural hint codified in `MATCHING_NOTES.md` make
the *next* revisit cheaper.

### Fresh iteration (default)

1. `tools/find_siblings.py --unmatched | head -30` — highest-payoff
   clusters first. **Sibling clusters are the unit of work, not single
   functions.** A cluster's value comes from porting one matched
   donor's C body across all siblings with target swaps; do this in a
   single iteration, not one function per iteration.
2. Filter out anything already listed in `docs/MATCHING_NOTES.md`'s
   tough-nut / deferred sections — those go through the revisit path.
3. Prefer a small sibling cluster (1–4 functions). Adjacent-c-c
   consolidation passes are an alternative — pick those when nothing
   matchable stands out.
4. If `find_siblings` returns nothing actionable: do an adjacent-c-c
   consolidation pass per `STARTING_PROMPT.md` instructions. Exit clean
   when done.

Per-iteration cap: **one cluster** (all matchable siblings in it) OR
**one revisit** (one deferred entry) OR **one consolidation pass** OR
**one recovery**. Land every sibling that matches before exiting; do
**not** stop at the first match in a cluster. If the donor body lands
on function A but fails on B in the same cluster, defer B specifically
(document its failure in `MATCHING_NOTES.md`) and still commit A.

## 4. Match attempt

### Toolkit (use these — do not reinvent)

Run `--help` on any unfamiliar tool before first use in this session.

- `tools/find_siblings.py --unmatched` — sibling-cluster finder
  (target selection above).
- `tools/find_matched_neighbors.py <addr>` — locate an already-matched
  donor function whose C body can be ported onto an unmatched sibling.
  **Use this whenever you've identified a cluster** — porting a
  validated donor is the single highest-yield move.
- `tools/scan_kr_signals.py <asm_path>` — K&R sub-variant histogram;
  identifies which K&R shape (int spill, struct-by-value, byte-narrow,
  float-promotion) the asm prologue indicates.
- `tools/which_si_register.py <addr>` — `$s0..$s7` usage profile +
  matched neighbours by register-overlap. Useful when no direct
  sibling exists but a function with similar callee-saved usage does.
- `tools/diff_objs.py <addr_a> <addr_b>` — twin-function diff with
  prologue buckets; spot which slots changed between donor and target.
- `tools/match_check.py 0xROM_OFFSET:0xSIZE` — per-function byte diff.
  Note: **ROM offset, not VRAM** — silent false MATCH past EOF.
- `tools/diagnose_match.py` — near-miss triage on the build artefact.
- `tools/check_yaml_order.py --fix` — splat ordering fixer if you've
  inserted a new `[addr, c, name]` line out of order.
- `timeout 300 tools/permute_run.sh <vram> <size> <seed>` — permuter
  driver (5-min budget on fresh iter, 1800s on revisit).

### Per-cluster workflow

For a cluster of N siblings:

1. Use `find_matched_neighbors.py` against each cluster member to
   surface a donor. If found, port the donor's C body — this is the
   fastest path to landing N matches in one iteration.
2. Edit `config/snap.us.yaml` to declare `[0xADDR, c, name]` for the
   first sibling (or a shared filename if all N will live in one .c
   per `STARTING_PROMPT.md` adjacent-merge rules).
3. `make split` to migrate asm under `asm/nonmatchings/`.
4. Write `src/<name>.c` for the first sibling (saved-feedback-memory
   tricks first; `decomp/NOTES.md` for codegen patterns).
5. `make` and inspect SHA-1. If MATCH on sibling #1, replicate the
   shape for siblings #2..#N with target swaps. Each replica is a
   `make` cycle — keep going until either every sibling matches or a
   sibling resists.
6. Resisting siblings: defer **just that one** (revert its yaml line,
   delete its src/, write the failure mode into `MATCHING_NOTES.md`).
   The siblings that did match still land in this iteration's commit.

If the first build is MISMATCH after a reasonable best effort (saved
memory tricks applied, a few obvious source-shape variations tried)
and the diff is operand-order / regalloc / scheduling shaped, run the
permuter with the budget appropriate to the iteration mode:

```sh
# Fresh iteration: throughput-biased
timeout 300  tools/permute_run.sh <vram> <size> /tmp/<name>.seed.c

# Revisit iteration: leverage-biased
timeout 1800 tools/permute_run.sh <vram> <size> /tmp/<name>.seed.c
```

**Hard cap.** Permuter exit 0 = matched (copy the permuter's source.c
back into `src/`). Non-zero on a fresh iteration = defer. Non-zero on
a revisit = update the deferred entry's notes with the new lowest
plateau score and best-candidate structural hints, then re-defer.

## 5. Resolution

### Matched

Stage and commit. Files to consider:

- `src/<name>.c`
- `config/snap.us.yaml`
- `config/pokemonsnap.us.ld` (auto-regenerated)
- `config/undefined_funcs_extra.us.txt` (auto-regenerated)
- `asm/nonmatchings/<name>/` removals (splat will drop these)
- Any `tools/` or `docs/` edits motivated by this iteration

`make` final SHA-1 must match `edc7c49cc568c045fe48be0d18011c30f393cbaf`
before committing. The pre-commit hook runs `tools/check_no_rom.sh`;
if it blocks, do **not** force the commit — treat as a recovery
condition: revert and exit.

### Deferred (no match in budget)

- `git restore config/snap.us.yaml`
- `rm src/<name>.c` (and any `tools/decomp-permuter/runs/*` debris is
  fine to leave — the wrapper cleans matched targets only)
- Update `docs/MATCHING_NOTES.md` per the "Building leverage on tough
  nuts" rules in `STARTING_PROMPT.md`: failure mode, what IDO emitted,
  permuter plateau score, best-candidate structural hints.
- `make split && make` to confirm green.
- Commit **only** the doc edit — the lesson is the value.

## 6. Append RUN_LOG.md

After the commit (or after recovery exit), append exactly one row to
`decomp/RUN_LOG.md` (the file is gitignored — do **not** stage or
commit it). Format:

```
<ISO-8601-Z> | <iter|revisit|recover> | <target-or-"-"> | <matched|deferred|recovered|consolidated> | <commit-sha-short-or-"-"> | <permuter-plateau-or-"-"> | <one-line note>
```

Examples:

```
2026-05-04T12:34:56Z | iter | func_8011AB80 | matched | a1b2c3d | - | sibling of 8011AB30, K&R int spill
2026-05-04T12:51:02Z | iter | func_8012CD40 | deferred | - | 188 | mul.s operand-order, plateau resists arg^0 trick
2026-05-04T13:02:18Z | recover | func_8013E0F0 | recovered | e4f5g6h | - | post-match commit landed on retry
2026-05-04T14:18:33Z | revisit | func_8012CD40 | matched | b7c8d9e | - | newly-saved bnel-do-while trick fit; 5th attempt at this entry
2026-05-04T15:02:11Z | revisit | func_8014A220 | deferred | - | 142 | plateau dropped from 188; new hint: chained-assignment in best-candidate
```

If `decomp/RUN_LOG.md` doesn't exist, create it with a header line:
`# Headless run log — gitignored, append-only`. Then append.

## 7. Exit

Working tree must be clean (`git status` empty). `make` must exit
MATCH at the target SHA-1. Then end the session — no further tool
calls, no summary chatter, no ScheduleWakeup.

If at any point you cannot satisfy these exit conditions and a clean
revert isn't reaching them, append a `failed` row to RUN_LOG.md
explaining the state, leave the tree as-is, and exit. The shepherd's
next iteration will run the session-start sanity check and recover.

**Wrap-up if Claude usage cap is about to hit mid-iteration:** follow
the rules in `STARTING_PROMPT.md` "Wrap-up if your Claude session usage
is about to run out" — land what you have, don't leave the build red.
