# func_80119F00 — byte-by-byte buffer swap

```c
do {
    tmp = *src; *src = *dst; *dst = tmp;
    src++; dst++;
} while (count-- != 0);   // approximation
```

Returns the running `count` value as it decrements (an oddity — `v0`
gets set to `a2` inside the loop body each iter, so the return is
the count value AT THE LAST ITERATION).

## What's matching (16 vs 14)

`count-- != 0` post-decrement do-while form gets close:
- if-skip-on-zero present
- byte loads + post-increment present
- final swap-write `sb v1, -1(a1)` present

## Diff (2 extra insns)

Mine emits `move a3, a0` (preserves dst into a3) and uses a3 as the
cursor; target uses a0 directly. Plus `sltu t0, zero, a2` for the
loop-exit test, where target uses bnez. The bnez+delay-slot
`a2-- (delay)` pattern target uses doesn't emerge from any
straightforward C — IDO is converting the post-dec do-while into an
sltu-based test.

## Tried

- `while(count != 0)` body-with-decrement: massively unrolled (40 insns)
- `if (count==0) return; do{...; count--;} while(count != 0)`: also unrolled
- `count-- != 0` post-decrement do-while: closest (16 insns), still off

## Permuter hint

Pure scheduling/regalloc — needs permuter to find the bnez+delay-slot
form. Or there's a non-obvious source idiom that produces target's
shape.
