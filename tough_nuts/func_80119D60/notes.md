# func_80119D60 — LCG random (multiplier 0x343FD, increment 0x269EC3)

**ROM offset:** `0x1A960` **size:** `0x50` (20 instructions)
**VRAM:** `0x80119D60`

## Behaviour
Classic linear-congruential RNG. Identical to libultra's `osGetRand`:
```c
state = state * 0x343FD + 0x269EC3;
return (state >> 16) & 0xFFFF;
```

State stored at `D_80040F40`.

## What's matching
Function semantics + shift/and at the end. Address materialization
of `D_80040F40` into a base register used for both load and store
(twin-lui-CSE-into-single-reg pattern).

## Remaining diff (almost everything)
**Multiplication codegen:** base uses a 10-instruction shift-add
chain to compute `state * 0x343FD`; built emits `lui+ori+multu+mflo`
(4 instructions). IDO 7.1 at `-O2 -mips2` emits multu for large
constants where shift-add chain is longer than ~5-6 instructions.
The original ROM was apparently compiled with a different
threshold/version that produced shift-add.

The shift-add chain:
```
t7 = state*4 - state           ; 3*state
t7 = t7*4 + state              ; 13*state
t7 = t7*16 + state             ; 209*state
t7 = t7*256 - state            ; 53503*state (= 0xD0FF)
t7 = t7*4 + state              ; 214013*state (= 0x343FD)
```

## Variants tried
- `s32` (signed) vs `u32` (unsigned) state: same multu emission.
- `(state * 0x343FD)` direct: emits multu.
- **Explicit shift-add chain** (saved as `func_80119D60.c` here):
  emits the right shift-add chain but with fresh registers per step
  (`$t6→$t7→$t8→$t9→$t0→$t1`) where base uses `$t7` throughout.
  Plus the state variable lands in `$v1` instead of base's `$t6`.

## Permuter hint
The shift-add form clears the `multu` issue. Remaining diff is pure
register-allocation: state should be in `$t6`, intermediate `r` in
`$t7`. Permuter should swap easily.

Possibly cracks via `register s32 t` or by using `*` with the same
literal `0x343FD` after some other tweak that makes IDO prefer the
chain (the original source likely just wrote `state * 0x343FD` and
got the chain "for free" from a different opt knob).
