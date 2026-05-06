# func_8012CD2C — ldexp-style: x * (1 << n) for double + s32

## Behaviour
```c
f64 func_8012CD2C(f64 x, s32 n) {
    if (n != 0) {
        s32 shift = 1 << n;
        f64 m = (f64) shift;
        x *= m;
    }
    return x;
}
```

## What's matching (8/9 instructions)
- beqz a2 with delay slot li t6,1
- sllv + mtc1 + cvt.d.w
- mul.d f12, f12, f6 (correct fs/ft order — `*=` operator unlocks this)
- jr ra; mov.d f0, f12 in delay

## Remaining diff (1 instruction: missing nop)
Target has a `nop` at offset 0x1C between the `mul.d f12, f12, f6` and
the `jr ra` (effectively making the if-body 4 instructions long instead
of 3). Mine packs jr ra immediately after mul.d.

The nop is FPU-pipeline-stall avoidance (mul.d → mov.d on the same
register without latency cushion). IDO emits it for some versions and
not others — appears to depend on scheduling heuristics.

## Variants tried
- `x = x * y` form: `mul.d $f12, $f6, $f12` (operands reversed — score 5)
- `x *= y` form: matches operand order but missing nop (closest, 1 inst diff)
- Explicit `f64 result = x; result *= ...; return result`: regresses
  (introduces extra mov.d f2 = f12 before the mul)

## Permuter hint
Try forcing scheduling via dummy fp ops or explicit `(volatile f64)` —
the nop is a pipeline padding artifact, not a source-level construct.
