# func_80130018 — exponentiation by squaring

Computes `arg0 * pow(arg2, arg1>>3)` capped at 32 squaring iterations.

## What's matched

Prologue (mtc1, sra, bnez early-exit, lui+f2 init), epilogue (mul.s
f0, jr ra, nop), and most of the loop body match byte-for-byte.

## What diffs (~3-4 inst, scheduling-only)

The loop's branch-back uses `bne` instead of `bnel`:

- mine:   `bne v0,v1, top` with `mul.s f14, f14, f14` in delay slot
- target: `bnel v0,v1, top` with `andi t6, a1, 1` in delay slot
          plus a `nop` padding inside the conditional-mul if-body

The original code recomputes `arg1 & 1` in the **branch-likely delay
slot** so the bit-test only refreshes when we actually loop back. To
emit `bnel`, the source needs a structure where `arg1 & 1` is computed
once before the loop AND once at the loop-continuation point — but
IDO -O2 keeps DCE'ing the explicit `bit = arg1 & 1` recompute even
when written as a separate local.

Tried (all collapse to same `bne` + `mul.s` delay slot):

- explicit `bit` local with pre-loop init + tail recompute
- `do { ... } while (++i != 32)` vs `i++; ... while (i != 32)`
- explicit `goto`-based loop with break out of `while (1)`
- `if (cond) break` vs natural while-condition

Permuter target — small score, scheduling-only.
