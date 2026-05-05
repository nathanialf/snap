# func_8011E99C — 3-arg deg-to-rad scaler + forward to func_8001D648

```c
func_8001D648(arg0, arg1, arg2, arg3,
              arg4 * D_80043DCC / 180.0f,
              arg5 * D_80043DCC / 180.0f,
              arg6 * D_80043DCC / 180.0f);
```

## Same diff as func_8011E8C0

Target loads `D_80043DCC` (the per-axis scale constant) BEFORE the
`0x43340000` (180.0f) constant. Mine reverses. 28-vs-28 instructions,
identical otherwise.

This is a load-order pattern that affects all 3-axis-scale siblings
(func_8011E8C0, func_8011E924, func_8011E99C, func_8011EA0C).
1-axis-scale siblings (func_8011E720/774/7DC/844, all matched)
don't have this diff.

## Permuter hint

Pure scheduler-level lui order. Permuter should crack the 1-axis
trick once, and the recipe applies to the whole 3-axis family.
