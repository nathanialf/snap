# func_8011A5F4 — vec3 *= s1; vec3 += vec * s2; norm(vec3) (mul.s nop)

Same family as func_8011A530's nop-between-mul scheduling. Builds
match in calls and value flow but base inserts a nop between the
2nd and 3rd `mul.s` (arg0[1]*=arg1 and arg0[2]*=arg1) that built
doesn't.

## Permuter hint

Same as func_8011A530 — find a way to force the nop spacer. May
be an FPU pipeline schedule that requires a pin or a specific
form of source not yet identified.
