# func_80119E78 — float scaler with epilogue mul (5/14 scheduling diff)

`return func_8003D478(0, (u8) func_80037EE0()) * (1.0f/256.0f);`
where func_80037EE0 returns u64 (the (u8) cast extracts the low
byte from v1, which holds the low 32 bits of the u64).

## Diff (5/14)

Body matches; only the epilogue order:
  - Base: lui at; mtc1 f4; lw ra; addiu sp; mul.s f0; jr ra; nop
  - Built: lw ra; lui at; mtc1 f4; addiu sp; jr ra; mul.s f0 (delay)

Mine schedules the mul into the jr-ra delay slot. Base does the
mul BEFORE the jr.

## Variants tried

- Inline `func_8003D478(...) * (1/256)` — delay-slot mul
- Local temp `f32 r = func_8003D478(...); return r * (1/256);` — same

## Permuter hint

Try forcing mul.s out of delay slot via a stack spill or extra
side effect that breaks scheduling.
