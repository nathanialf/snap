# func_8011A5F4 — Vec3 scale + offset, then forward to func_8001A360

```c
v[0] *= scale; v[1] *= scale; v[2] *= scale;
v[0] += offset_src[0] * offset_scale;
v[1] += offset_src[1] * offset_scale;
v[2] += offset_src[2] * offset_scale;
func_8001A360(v);    /* tail forward */
```

Floats `scale` and `offset_scale` arrive in $a1 and $a3 (int regs)
because the first arg is a pointer (MIPS o32 ABI quirk). IDO emits
`mtc1 a1, f12` and `mtc1 a3, f14` to use them in FP ops.

## Diff: nop placement between back-to-back muls

Target inserts a nop between mul2 and mul3 of the scaling phase:

```
mul.s f6,  f4,  f12   ; v[0] * scale
mtc1  a3, f14
mul.s f10, f8,  f12   ; v[1] * scale
nop                   ; <-- target has this
mul.s f18, f16, f12   ; v[2] * scale
```

Mine has 3 back-to-back muls and trailing nop padding instead.
Same total instruction count (34); just different nop placement.

The reload-after-store pattern (`swc1 f6, 0(a0); lwc1 f4, 0(a0)`) is
correctly produced by the natural `v[0] *= scale; ...; v[0] += ...;`
two-phase form — no `volatile` needed.

## Try later

- Permuter scheduling perturbations
- Try `f32 s = scale; ... s ...` to see if a local copy changes scheduling
- Look at IDO 5.3 vs 7.1 scheduling differences
