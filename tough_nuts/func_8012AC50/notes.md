# func_8012AC50 — fixed-point multiply with rounding

**ROM offset:** `0x2B850` **size:** `0x4C` (19 instructions)
**VRAM:** `0x8012AC50`

## Behaviour
```c
return (s32) (f32) ((f64) ((f32)arg1 * (f32)arg0[17]) / D_800442B0 + 0.5);
```

D_800442B0 is an `extern f64` constant (probably `(f64)65536.0` for
Q16 fixed-point divide).

The 0.5 added before truncation = round-to-nearest. Note `0x3FE00000`
is the high half of double `0.5`; built via `mtc1 $zero, $f6;
mtc1 $at, $f7` (low/high halves of $f6:$f7 double pair).

## What's matching
- Mathematical structure (cvt.s.w, mul.s, cvt.d.s, div.d, add.d,
  cvt.s.d, trunc.w.s, mfc1)
- D_800442B0 reference loads (ldc1 from %lo)
- 0.5 double materialization (lui 0x3FE0, mtc1)

## Remaining diff (10/19 instructions)
**FP register-allocation swap.** Base assigns:
- `arg1` (cvt'd) → `$f4`/`$f6`
- `arg0[17]` (cvt'd) → `$f8`/`$f10`

Built reverses: arg1 → $f8/$f10, arg0[17] → $f4/$f6.

Plus a scheduling diff in the prologue where built has both cvt.s.w
instructions BEFORE the `ldc1`, while base interleaves them with
the load.

## Variants tried
- Reordered `(f32)arg1 * (f32)arg0[17]` to `(f32)arg0[17] * (f32)arg1`
  (commutative) — no change.
- Cast variations: `(f64)((f32)arg1)` first, etc.

## Permuter hint
Pure FP-regalloc diff. Permuter should crack quickly.
The constant 0x3FE00000 + double 0.5 + Q16 divide pattern is likely
common across the codebase — sibling functions probably share the
same shape and would benefit from the same recipe.
