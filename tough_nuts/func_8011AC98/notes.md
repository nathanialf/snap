# func_8011AC98 — vec3 reflect (mul.s f-reg choices)

`arg0 += arg1 * (-2 * dot(arg1, arg0))` — vector reflection. Math
correct but fp-register assignments diverge across the dot product
and the trailing FMA chain.

## Permuter hint

Many fp-reg variants. Likely permuter-only.
