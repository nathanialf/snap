# func_801322D0 — pow(D_80044880_or_4884, n) by squaring

Selects base = D_80044884 if n<0 (also negates n) else D_80044880,
then exponentiates by squaring.

## Diff (~4-5/22 instructions)

Same structure as parked func_80130018 (different exp-by-squaring):

1. **bnel pattern in inner loop**: target uses `bnel a0, 0, .top`
   with `andi t6, a0, 1` in the delay slot (re-compute on continue);
   mine uses `bnez a0, .top` with `mul.s f0, f0, f0` (base squaring)
   in delay slot and the andi at loop top each iter.
2. **Missing nop after `mul.s f2, f2, f0`**: target has it as
   if-body padding; mine packs tighter.
3. **bgez vs bltz**: target tests `bltz a0` (n<0 branch is taken);
   mine emits `bgez a0` (the inverse). IDO chose the inverse fall-
   through; source-level inversion (writing the if as
   `if (n >= 0) ... else { n=-n; ... }`) doesn't flip it back.

## Permuter hint

Same permuter category as func_80130018 — the bnel-with-update-in-
delay-slot pattern needs IDO to recognize `andi t6 = a0 & 1` as the
loop-continue computation rather than a top-of-iteration test. Try
declaring t6 explicitly as `s32 bit;`, computing it both before the
loop AND at the tail of the body, with the loop guarded by an outer
if so the do-while is wrapped:
  `if (n != 0) { do { ... bit = n & 1; } while (n != 0); }`.

If func_80130018 ever cracks, apply the same recipe here.
