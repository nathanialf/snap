# func_80119BBC — asin-like float clamp + body (bc1f vs bc1fl)

Float function that clamps arg0 against two limits (D_80043D68 high
and D_80043D70 low) returning constants D_80043D6C / D_80043D74,
otherwise computes
`func_80019994(arg0 / func_80036090(1.0f - arg0 * arg0))`.

## Diff (2/33)

The second `c.lt.s + bc1f + nop` test compiles to bc1fl (branch
likely) with `mul.s f10, f14, f14` hoisted into the delay slot,
instead of base's `bc1f` + `nop` with the mul AFTER the branch
target. Functionally identical; just IDO's branch-likely pick.

```
1A7FC  base 45000003 (bc1f .L80119C0C)  built 45020004 (bc1fl)
1A800  base 00000000 (nop, delay)        built 460e7282 (mul.s)
```

## Variants tried

- Inline `1.0f - arg0 * arg0` — bc1fl
- Local temp `f32 t = arg0 * arg0;` — bc1fl

## Permuter hint

Try a pre-test that forces IDO out of branch-likely; e.g. compute
the multiplication in a separate *prior* statement that's used
elsewhere, or split the early-return cases into a separate helper.
