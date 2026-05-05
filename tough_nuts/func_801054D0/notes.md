# func_801054D0 — 16-byte struct copy from indexed array

`D_8004A8B8 = D_8004A928[D_8004A910]; func_80007850();` — copies a
4-int Vp16-style struct from a global array indexed by a global to
a single global, then calls a no-arg function.

## Diff

Same family as func_80100EBC: struct copy gets correct shape but
chooses different starting registers (t7 vs t6 for src-base, etc.)
and the prologue lands earlier (mixed with the index compute) in
built vs base.

## Permuter hint

Try a memcpy-style call or a static helper that takes (dst, src)
to coax the a0/t6 reg routing of base.
