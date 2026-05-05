# func_80119BBC — clamped piecewise math (likely asin/atan2 fallback)

```c
if (x > D_80043D68) return D_80043D6C;
if (x < D_80043D70) return D_80043D74;
return func_80019994(x / func_80036090(1.0f - x * x));
```

## Diff: bc1fl-with-mul.s vs bc1f+nop

For the **second** early return (`if (x < D_80043D70) return D_80043D74;`)
target uses non-likely branch:

```
bc1f .L80119C0C
 nop
b .L80119C30
 lwc1 f0, lo(D_80043D74)(at)
.L80119C0C:
mul.s f10, f14, f14   ; first instruction of else
```

Mine emits branch-likely with mul.s speculatively in delay slot:

```
bc1fl 0x54
 mul.s f10, f14, f14   ; (executes only if branch taken — speculation!)
b 0x74
 lwc1 f0, lo(D_80043D74)(at)
mul.s f10, f14, f14   ; second copy for fall-through
```

The first early return matches exactly (bc1f + nop). Only the second
gets the `bc1fl` form. IDO's heuristic for choosing likely vs non-likely
seems to depend on something I can't discern from the C source alone.

## Try later

- Permuter for branch-style permutations
- Restructure as nested if/else to see if it changes the second branch type
- Single-return form with one local + falling-through assignment
