# Tough nuts — deferred near-miss attempts

Each subdir holds a function whose source body is *almost* matching but
left a few bytes different (typically register-allocation, instruction
scheduling, or operand-order diffs that source-side rewrites alone
won't dislodge). Files here are **not compiled into the build** — they
exist so the operator can point the decomp-permuter at them later,
out-of-session, without dragging a half-broken source through `src/`.

Layout:

```
tough_nuts/<func_name>/
    <func_name>.c        # best source-side attempt
    notes.md             # ROM offset:size, remaining diff, variants tried
```

To run the permuter against one:

```sh
tools/permute_run.sh <vram_addr> <size> tough_nuts/<func_name>/<func_name>.c
```

If it lands a match, move the file into `src/`, flip the yaml entry
from `asm` to `c, <func_name>`, and commit normally.

This directory is gitignored from the build (no Makefile reference),
but the source files themselves ARE checked in — they're contributor
work product, not generated artefacts.
