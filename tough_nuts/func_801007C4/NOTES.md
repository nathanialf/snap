# func_801007C4 — boot/thread setup with magic-canary write

```c
func_80025C38(arg0);
func_80034A00(D_80046880, 5, D_800005D8, arg0, &D_80047A30, 0x32);
D_80046A68 = 0;
D_80046A6C = 0xFEDCBA98;       /* magic canary */
if (D_80040CE4 == 0) {
    func_8003D0F0(D_80046880);
}
func_80037A10(0, 0);
for (;;) ;
```

## Diff: 0 stored via t8 register vs sw zero

Target writes 0 to D_80046A68 by first loading 0 into t8 then `sw t8`,
and pre-computes 0xFEDCBA98 into t9 BEFORE the bnez:

```
lw    t0, D_80040CE4
lui   t9, 0xFEDC
ori   t9, 0xBA98
addiu t8, zero, 0           ; <-- 0 in t8
lui   at, hi(D_80046A68)
sw    t8, lo(D_80046A68)(at)  ; sw t8 (not sw zero!)
bnez  t0, .L80100834
 sw   t9, lo(D_80046A6C)(at)  ; delay slot
```

Mine emits `sw zero, lo(D_80046A68)(at)` (preferring zero reg) and
generally interleaves the load+store sequence differently. Tried:
- Reverse declaration order — got reverse store order, not target's
- `volatile u32 D_80046A68` — bloats codegen, bnez→bnezl, doesn't help

## Hypothesis

Source might use an intermediate variable for the 0 value (a struct member
or something the compiler can't optimize to "just write zero"), or some
specific aliasing that makes IDO materialize 0 in a register first.

## Try later

- Permuter for register allocation perturbations
- Look at neighboring boot code for D_80046A68's actual access pattern
  (could be a struct field elsewhere)
