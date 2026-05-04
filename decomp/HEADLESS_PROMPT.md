# Headless multi-match session prompt

This file is the prompt fed to each `claude --print` invocation by
`tools/headless_run.sh`. **You are an unsupervised matching session.**
Match as many functions as you can in this session. Exit cleanly when
your Claude usage cap is about to hit; the shepherd will start a fresh
session after the cap window resets.

**Hard rules:**

- Do not start `/loop`. Do not call `ScheduleWakeup` or `CronCreate`.
- Do not ask the user questions — there is no user attached.
- Do not push to any remote. Do not bypass git hooks (`--no-verify`).
- Do not add an AI co-author trailer.
- Respect the IP rules in `CLAUDE.md` (auto-loaded). The
  `tools/check_no_rom.sh` pre-commit hook will block bad commits — do
  not work around it.
- Do not modify `tools/headless_run.sh` or this file
  (`decomp/HEADLESS_PROMPT.md`) from inside a session.

## Context that's already loaded — don't re-read

When this session starts, the harness has already loaded:

- `CLAUDE.md` — project memory (IP rules, build commands, IDO setup).
- Saved-feedback memory under
  `~/.claude/projects/-primary-dev-snap/memory/feedback_snap_*.md` —
  K&R sub-variants, volatile-pointer trick, struct-by-value,
  byte-narrow-via-memory, K&R float promotion, bnel do-while,
  `mul.s` operand-swap, leverage strategy, wrap-up rules.

You **do not** need to `Read` these files at session start. Doing so
burns API time and the user's usage cap on context that's already in
your system prompt. Reference them only if you need to refresh a
specific detail mid-session.

Read these on-demand (not at session start):

- `decomp/NOTES.md` — IDO/splat/linker codegen patterns. Open it when
  a near-miss diff smells like a known pattern you can't recall the
  recipe for.
- `docs/MATCHING_NOTES.md` — tough-nut entries / deferred targets.
  Open it when you're about to defer (need to log a failure mode), or
  when picking a revisit target.
- `decomp/STARTING_PROMPT.md` — the interactive-session prompt; the
  workflow rules live there in fuller form. Reference for the leverage
  strategy or the consolidate-adjacent-c rules — not required reading.

Required at session start (cheap, ~1s):

- `git status` — confirm clean entry state.
- `git log -20 --oneline` — see what the previous session committed.

## Session-start sanity check

If `git status` is **dirty**, the previous session was killed
mid-flow. Recover before doing anything else:

- Run `make`. If MATCH: stage `src/`, `config/`, `decomp/`, `docs/`,
  `tools/` as relevant, commit (the previous session matched but died
  before committing). Append a `recovered` row to `decomp/RUN_LOG.md`.
- If MISMATCH or build-red: `git restore config/snap.us.yaml`,
  `rm src/<orphaned>.c`, `make split && make` to confirm green.
  Append a `recovered` row to RUN_LOG.md.
- Then proceed to the matching loop below.

## Matching loop (the actual work)

Match function clusters until your usage cap is about to run out.

### Per-match cycle

1. **Pick a target.** `tools/find_siblings.py --unmatched | head -30`.
   **Sibling clusters are the unit of work, not single functions.**
   Filter out anything in `MATCHING_NOTES.md`'s tough-nut / deferred
   sections — those go through the revisit path below. Prefer small
   sibling clusters (1–4 functions). If `find_siblings` returns
   nothing actionable, do an adjacent-c-c consolidation pass per
   `STARTING_PROMPT.md` rules.

2. **Find a donor.** `tools/find_matched_neighbors.py <addr>` against
   each cluster member. Porting a validated donor's C body is the
   single highest-yield move per cluster.

3. **Apply saved-memory tricks first.** The signals that should
   immediately pull a specific trick out of memory:
   - dead `sw $a0`/`sw $a1` spills with no later read → struct-by-value
   - prologue spills all args + `lbu` from spill+3 → K&R int param,
     byte-narrow via memory
   - `ldc1`/`cvt.s.d` on a float arg → K&R promotes f32 to double;
     switch the seed to ANSI prototype to suppress
   - `bnel` with update in delay slot → `if (cond) { do { update;
     } while (cond); }` not plain while
   - `mul.s` operand-order diff in a single-word body → rewrite as
     `LHS * (spill = RHS)` to flip fs/ft order
   - non-`$v0` temp ($t6, $t7…) holding a global → source is
     `*(volatile T *)ADDR`, not extern T

4. **Edit yaml + write seed.** Add `[0xADDR, c, name]` to
   `config/snap.us.yaml`. `make split`. Write `src/<name>.c`. `make`.

5. **If MISMATCH and shape is operand-order / regalloc / scheduling:**
   `timeout 300 tools/permute_run.sh <vram> <size> /tmp/seed.c`.
   5-minute hard cap. **Seed file MUST be ANSI prototype style** (the
   wrapper pre-flights for K&R param decls and bails). Permuter exit
   0 = matched (copy the permuter's source.c back into `src/`).
   Non-zero = defer.

6. **For each matched sibling**, replicate the donor's shape with
   target swaps and `make` until either every sibling matches or one
   resists. Resisting siblings get deferred individually.

7. **Stage + commit.** Files: `src/<name>.c`, `config/snap.us.yaml`,
   `config/pokemonsnap.us.ld` (auto-regen), `config/undefined_funcs_extra.us.txt`
   (auto-regen), removed `asm/nonmatchings/<name>/`. Final SHA-1 must
   be `edc7c49cc568c045fe48be0d18011c30f393cbaf`. Commit message
   follows the repo's existing style — see `git log` for examples.

8. **Defer path** (no match in budget):
   - `git restore config/snap.us.yaml` and `rm src/<name>.c`.
   - Update `MATCHING_NOTES.md` per the tough-nut leverage rules:
     failure mode, what IDO emitted, permuter plateau score,
     best-candidate structural hints. The lesson is the value.
   - `make split && make` to confirm green.
   - Commit only the doc edit.

9. **Append RUN_LOG.md.** One row per match, defer, recover, or
   consolidation. Format:

   ```
   <ISO-8601-Z> | <iter|revisit|recover> | <target> | <matched|deferred|recovered|consolidated> | <commit-sha-short-or-"-"> | <permuter-plateau-or-"-"> | <one-line note>
   ```

   `decomp/RUN_LOG.md` is gitignored — do NOT stage or commit it. If
   it doesn't exist, create with a header line then append.

### Revisit cadence (cracking tough nuts)

Every ~6 successful matches, do **one** revisit instead of a fresh
cluster. Also trigger a revisit opportunistically when this session
just landed a match that introduced new leverage (a saved-memory
trick, a `decomp/NOTES.md` pattern, or a sibling-family crack near a
deferred entry's address).

Revisit procedure:

1. Read the deferred / tough-nut sections of `MATCHING_NOTES.md` in
   full. Pick the entry whose failure mode plausibly matches recent
   leverage. Prefer entries with the lowest recorded permuter plateau
   score — those are closest to cracking.
2. Try multiple source-shape variations (3–5) before falling back to
   the permuter. Run through every saved-memory trick that could
   plausibly fit.
3. If the permuter is needed, use the **revisit budget**:
   `timeout 1800 tools/permute_run.sh <vram> <size> <seed>` — 30-min
   wall-clock cap.
4. Inspect the permuter's best-candidate `source.c` even on non-zero
   exit; the synthetic `if (1)` / chained-assignment / dummy-temp
   shapes are hints about IDO's scheduling state.
5. If matched: commit + RUN_LOG row tagged `revisit`. If still
   deferred: update the entry's notes with new lowest plateau score
   and best-candidate structural hints, commit only the doc edit, log
   `revisit … deferred`.

A revisit that doesn't crack still has value. The next session reads
the updated notes.

### Recognise libultra-shaped functions

`lib/ultralib/` is cloned (gitignored, per CLAUDE.md IP exception)
but **not yet wired into the build as a matching target**. When you
see asm signalling a libultra primitive — `cfc1`/`ctc1` on `$31`,
`mfc0`/`mtc0` on system regs, cache ops, osMesgQueue / osThread /
osEvent constant patterns — defer immediately with a
`MATCHING_NOTES.md` entry naming the suspected libultra source
(e.g. `ultralib/src/os/fpu/setfpccsr.c`). Don't burn permuter
cycles. Cracking the whole libultra range is a separate workstream
that will unblock dozens of these at once.

### When to stop and exit cleanly

Watch your usage cap (the rate-limit field is visible in the status
line). When you have **less than 15 minutes of budget left** in the
5h window, stop picking new targets and:

1. If a match is mid-attempt: complete it if cheap, otherwise revert
   (`git restore config/snap.us.yaml`, `rm src/<orphan>.c`,
   `make split && make`) so the tree is clean.
2. If you discovered a new pattern this session, codify it in
   `decomp/NOTES.md` "IDO codegen patterns to recognise" or open a
   PR for a saved-feedback memory file. Commit before exit.
3. `git status` must be empty. `make` must be MATCH. Then exit.

If at any point you can't reach a clean exit state and a clean revert
isn't getting you there, append a `failed` row to RUN_LOG.md with the
state, leave the tree as-is, and exit. The next session's
session-start sanity check will recover.

Do **not** chain into a new revisit when your budget is below 30
minutes — revisits can take 30+ min and you'll get killed mid-flow.
