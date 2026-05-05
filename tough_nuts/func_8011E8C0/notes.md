# func_8011E8C0 — 3-axis deg-to-rad scaler + forward to func_8001D26C

```c
func_8001D26C(arg0,
              arg1 * D_80043DC4 / 180.0f,
              arg2 * D_80043DC4 / 180.0f,
              arg3 * D_80043DC4 / 180.0f);
```

## Diff (load order)

Same instruction set/count as target (25 vs 25). Only diff: target loads
`D_80043DC4` into `f0` BEFORE loading the `0x43340000` (180.0f) constant
into `f2`. Mine reverses — loads 180.0f first.

```
target:                                  built:
lui at, hi(D_80043DC4)                   lui at, 0x4334
lwc1 f0, lo(D_80043DC4)(at)              mtc1 at, f2
mtc1 a1, f12                             lui at, hi(D_80043DC4)
mtc1 a2, f14                             ...
addiu sp, -0x18                          ...
mul.s f4, f12, f0                        ...
```

## Tried

- `arg1 * D_80043DC4 / 180.0f` (D in middle of expression)
- `f32 d = D_80043DC4; ...arg1 * d / 180.0f...`
- `D_80043DC4 * arg1 / 180.0f` (D first in expression)

All produce 180-loaded-first.

## Permuter hint

Pure scheduling — 1-line `lui` order swap. Permuter should crack
quickly. Sister of func_8011E720, func_8011E774, func_8011E7DC,
func_8011E844 (all matched). The 3-axis variant family extends with
more siblings at func_8011E924+ that will likely have the same
diff once cracked.
