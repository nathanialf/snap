# func_80119EB0 — multu + cvt.s.w + mul + trunc.w.s scale (4-insn reg diff)

`(s32) ((f32) ((u8) func_80037EE0() * arg0) * (1.0f/256.0f))`. Mixes
u64 return cast to u8, multiplied by signed arg, scaled by 1/256.

## Diff (4/20)

Body matches; only register names differ:
  - Base: arg0 reload via t9, multu t7,t9 → mflo t0 → mtc1 t0,f4
  - Built: arg0 reload via t8, multu t7,t8 → mflo t9 → mtc1 t9,f4

## Variants tried

- ANSI prototype
- K&R prototype

Both produce identical reg routing.

## Permuter hint

Pure regalloc; permuter should crack with a pin.
