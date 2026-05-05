# func_80108394 — freelist alloc with error-path layout

Pops a slot off the freelist at D_8004AAE8, lazily initialising it
via `func_800054A0(D_8004AC00, 8)` if empty, with two diagnostic
trap paths (out-of-budget at D_8004ABFC vs D_8004AC02, and a
"freelist still empty after alloc" check). Increments the in-use
counter D_8004ABFC.

## Diff (built vs base, structural)

Base lays the out-of-budget trap (`func_80025F24(D_800432E4)`) at
the end of the function, inline in the function body but reached
only via a forward branch from the limit check; the alloc path is
laid first. Built code emits the trap inline immediately after the
limit check, before the alloc, shifting every subsequent
instruction by ~6 slots.

```
0x8FA4  base 10410005  built 1041000b   ;; beq target offset
0x8FB4  base slt        built lui a0    ;; limit check vs early lui
0x8FB8  base beqz       built slt
...                                       ;; entire mid-section shifted
```

## Variants tried

- Combined `if (X != -1 && X <= Y) error;` — inline trap
- Nested `if (X != -1) { if (X <= Y) error; }` — inline trap
- Both produce same shape; IDO doesn't move the unreachable trap
  to the layout-tail.

## Permuter hint

Try a `goto err;` form with the `err:` label placed after the
return path; or wrap the trap in a separate function that's
inlined; or rewrite the limit check to compute a single boolean
into a register before branching, which may flip the layout.
