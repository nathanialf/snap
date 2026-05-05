# func_8010C274 — bucket walk + node-list cleanup (2-instr addiu swap)

For each bucket pointer in the array `D_8004A9E8..D_8004AA68`, walk
the chain via `p->next` and call `func_8000A52C(p)` on each node.

## Diff (2/28)

Body matches except the order of the two `addiu` lo-half loads:
  - Base: `addiu s3, s3, lo(D_8004AA68)` THEN `addiu s2, s2, lo(D_8004A9E8)`
  - Built: opposite order (s2 lo first then s3 lo)

Both functionally identical; just IDO scheduler preference.

## Variants tried

- do-while at bottom + bnel exit
- for-loop (worse — many more diffs)
- swap C declaration order (no effect)

## Permuter hint

Try a static helper that takes the end pointer first, or use the
end pointer as a function arg. May also clear with a permuter run.
