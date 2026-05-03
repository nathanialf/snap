# Unsupervised matching starting prompt

Reuse this verbatim (or paste it as your first message) to launch a fresh
Claude Code session for unsupervised matching work on this repo.

---

Continue the Pokémon Snap clean-room decomp at `/primary/dev/snap`.
Working directive: match as many ROM functions as possible
unsupervised, plug away at the percentage. Same rules as before — small
targets, sibling clusters preferred, consolidate adjacent siblings into
shared files when adjacent, semantic filenames only when the role is
unambiguous. Edit tools when you find errors and include them in your
commits. Stage `src/`, `config/`, `tools/`, `decomp/`, `docs/`, and
`README.md` as relevant. No AI co-author trailer.

**Read first** (in this order):

1. `CLAUDE.md` — IP rules, ultralib exception, build commands, workflow.
2. `decomp/NOTES.md` — every IDO 7.1 / splat / linker gotcha catalogued
   so far. Most "near-miss" diffs are an instance of a pattern in here.
3. `docs/MATCHING_NOTES.md` — per-function tough-nuts and deferred
   targets.

**Toolkit** (run `--help` on each before first use):

- `tools/find_siblings.py --unmatched` — next-highest-payoff clusters.
- `tools/scan_kr_signals.py` — K&R sub-variant histogram.
- `tools/find_matched_neighbors.py` — port a matched donor's C body
  onto a sibling unmatched function.
- `tools/diff_objs.py` — twin-function diff with prologue buckets.
- `tools/which_si_register.py` — `$s0..$s7` usage profile + matched
  neighbours by overlap.
- `tools/diagnose_match.py` — near-miss triage.
- `tools/check_yaml_order.py --fix` — splat ordering fixer.
- `tools/match_check.py 0xROM_OFFSET:0xSIZE` — per-function byte diff
  (note: ROM offset, not VRAM — silent false MATCH past EOF).
- `tools/permute_run.sh <vram> <size> <seed.c>` — decomp-permuter
  driver; cracks operand-order / regalloc / scheduling near-misses.
  Seed file MUST be ANSI prototype style (the wrapper now pre-flights
  for K&R param decls and bails with an error). The matched `src/` file
  itself can stay K&R — only the seed for the permuter needs ANSI.

**Per-cluster workflow:** identify shape via `find_siblings` → write one
C file → test with `match_check.py` → copy template across siblings
with target swaps → `make split && make` → commit. SHA-1 target:
`edc7c49cc568c045fe48be0d18011c30f393cbaf`.

**Long-running commands.** `make split` is ~75s and the permuter
typically runs for minutes. Run them with `run_in_background: true` and
poll with `Monitor` against the output file rather than blocking on
foreground `Bash`. Useful Monitor patterns:

- Permuter: `tail -f <permuter.log> | grep --line-buffered -E "score = 0|matched|wrote|Error|Exception"` — fires when the run finds a score-0 candidate, errors out, or exits. Don't grep only for success ("score = 0") — include error patterns so a pycparser crash or compile failure also surfaces, instead of going silent.
- Build watch: `until make 2>&1 | tail -3 | grep -q "MATCH\|MISMATCH"; do sleep 2; done` — emits when the build settles, with the verdict line.

If the monitor fires "timed out", that's the watch giving up — not a tool failure. Re-arm if the underlying job is still running, or check the output file directly with `Read`. Background `Bash` calls send their own completion event when the command exits; use those for one-shot waits and `Monitor` for streaming progress events.

**When a near-miss won't yield in 2-3 attempts:**
1. Check the saved feedback memories
   (`~/.claude/projects/-primary-dev-snap/memory/feedback_snap_*`) for
   a known pattern.
2. Check `decomp/NOTES.md` "IDO codegen patterns to recognise."
3. Try the permuter via `tools/permute_run.sh`.
4. If none of the above fit, defer to `docs/MATCHING_NOTES.md` "tough
   nuts" and pick a different cluster. Don't sink hours into one
   byte-level codegen difference.

**Spawning a tooling subagent.** If you hit the same kind of friction
3+ times in a session — same K&R sub-variant, same scheduling pattern,
same yaml-split footgun — and a small targeted tool would unblock the
broader workflow rather than one function, spin up a subagent to build
the tool. Threshold: "a non-one-off improvement across the workflow,"
not "one stuck function." Examples that would have qualified:
`scan_kr_signals.py` (K&R sub-variant histogram), `find_siblings.py`
(cluster-finder), `permute_run.sh` (permuter driver). Examples that
would NOT have qualified: a one-off script to fix the operand order in
a single `mul.s` (use the seed file + permuter directly).

When you spawn a tooling subagent, brief it with: the friction pattern,
3+ concrete instances of it, the IP rules from `CLAUDE.md`, and an
explicit instruction to leave `src/` / `config/` / `build/` untouched
and commit nothing — you'll review its output before integrating.

**Do not:**

- Commit `lib/ultralib/` (gitignored), `tools/decomp-permuter/`
  (gitignored), `asm/` (except `asm/nonmatchings/`), `assets/`,
  `build/`, or `baserom.us.z64`.
- Bypass the SHA-1 check by editing `config/sha1sums.txt` — bytes
  diverged, fix the bytes.
- Lift symbol names, code, or comments from leaked development
  materials. ultralib is the only Nintendo-derived material allowed
  (per `CLAUDE.md` exception).

**End-of-session expectation:** build green at SHA-1
`edc7c49cc568c045fe48be0d18011c30f393cbaf`, commits cleanly staged,
auto-regenerated artefacts (`config/pokemonsnap.us.ld`,
`config/undefined_funcs_extra.us.txt`) caught up to your yaml edits,
working tree clean.

**Wrap-up if your Claude session usage is about to run out.**
Unsupervised sessions can hit the rolling Claude usage cap
mid-iteration. Before that happens, *land what you have*:

1. If a function matches: stage + commit it immediately, don't batch.
2. If a function is mid-attempt and not matching: revert the yaml +
   delete the partial `src/<func>.c`. Don't leave the build red.
3. If you discovered a new pattern, trick, or deferred-class entry:
   write it into `decomp/NOTES.md` or `docs/MATCHING_NOTES.md` and
   commit *that* even if no match landed — the lesson is the value.
4. If a tool needs a fix you've already implemented: include
   `tools/` in the same commit as the matches that motivated it.
5. Final commit before stopping: ensure `git status` is clean and
   `make` is green at the target SHA-1. A failing build left in the
   tree blocks the next session before it can start.

When in doubt, prefer "stop one match early, commit, leave a clean
tree" over "squeeze in one more attempt and run out of budget
mid-edit."
