# func_8011AD94 — vec3 dot-product sign (mul/add scheduling)

`s32 sign_of_dot(vec3 a, vec3 b)` — returns -1/0/1 based on sign of
the dot product. Math is correct; scheduling of the mul.s + add.s
chain doesn't match base's interleaving of loads/multiplies.

## Permuter hint

Try various source orderings of the dot product expression:
  - `a[0]*b[0] + a[1]*b[1] + a[2]*b[2]`
  - `b[0]*a[0] + b[1]*a[1] + b[2]*a[2]`
  - With explicit local for each multiplication
