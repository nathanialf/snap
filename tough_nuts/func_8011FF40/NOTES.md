# func_8011FF40 — flag-guarded slot relocation

```c
if (arg0->_3 == 0) {
    arg0->_3 = 1;
    if (arg0->_E > 0) {
        s32 *p = (s32 *)arg0;
        s32 i = 0;
        do {
            void *r = (void *)(p[4] + arg1);
            p[4] = (s32)r;
            func_8001FFD8(r, arg1, arg2);
            i++; p++;
        } while (i < arg0->_E);
    }
}
```

Same structural pattern as func_8011FE08 (also parked) but with:
- `_3` byte flag as guard (set to 1 on entry)
- `_E` (s16) as count
- slots at offset 0x10 (p[4] in p-walking form)
- unconditional inner call (no `if (r != 0)` skip)

## Diff: t0 vs t1 for the count reload

Last remaining diff:
```
target: lh t1, 0xE(s3); ...; slt at, s1, t1
mine:   lh t0, 0xE(s3); ...; slt at, s1, t0
```

Just one temp register choice. Mine chose t0 (lower number); target picked
t1. Identical semantics, identical instruction count, identical layout
otherwise — purely IDO temp-reg allocator quirk.

The bigger structural diffs (duplicate-blez, s1/s2 swap for arg0/counter)
were fixed by:
1. `if (cond) { do ... while }` form (vs `if (!cond) return; for ...`)
2. Declaring `i = 0` BEFORE `s32 *p = ...` (delay-slot ordering)

## Try later

- Permuter for temp-register permutations
- See if the t1 use happens because of a slightly different access pattern
  (e.g., a redundant intermediate)
