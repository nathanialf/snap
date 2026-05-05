# func_8011E924 — 3-axis Mtx-build wrapper for func_8001D26C+func_8001B320

```c
f32 mtx[16];
func_8001D26C(mtx, arg1 * D_80043DC8 / 180.0f, arg2 * D_80043DC8 / 180.0f, arg3 * D_80043DC8 / 180.0f);
func_8001B320(mtx, arg0);
```

Same diff family as func_8011E8C0, func_8011E99C, func_8011EA0C: target
loads `D_80043DC8` BEFORE `0x43340000` (180.0f). Mine reverses.

## Permuter hint

Pure scheduler-level lui order. Same wedge as the rest of the 3-axis
deg-to-rad family. Cracking one cracks the family.
