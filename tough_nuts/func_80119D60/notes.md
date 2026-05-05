# func_80119D60 — MSVC-style LCG random (2-insn delay-slot order)

The classic Microsoft `rand()` LCG: state = state*214013 + 2531011;
return (state >> 16) & 0xFFFF.

D_80040F40 holds the state.

## Diff (2/20)

The body matches; only the very last two instructions are swapped:
  - Base: `andi v0, v0, 0xFFFF` then `jr ra; sw t8, 0(v1)` (delay)
  - Built: `sw t8, 0(v1)` then `jr ra; andi v0, v0, 0xFFFF` (delay)

Both functionally identical; IDO chose differently which one fills
the delay slot.

## Variants tried

- Single-line `D_80040F40 = D_80040F40 * 214013 + 0x269EC3; return ...`
  (uses shift sequence for *214013, but stores after andi)
- Local temp `s32 t = ...; D_80040F40 = t; return (t >> 16) & 0xFFFF;`
  (regresses — IDO uses multu instead of shift sequence)

## Permuter hint

Try forcing the andi to be a separate variable that's used after
the assignment to D_80040F40, or split the return chain so the
`& 0xFFFF` mask happens explicitly before the global write.
