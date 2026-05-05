# func_8010785C — align-up bump allocator with overflow trap

A bump-pointer allocator: aligns `arg0[3]` (current cursor) up to
`arg2` boundary if non-zero, advances by `arg1`, and traps if the
new cursor exceeds `arg0[2]` (max). On overflow it calls
`func_80025F24(D_80043260, arg0[0])` and spins in `while (1) {}`.

Returns the (pre-advance) aligned cursor.

## Closest seed: `arg2--; v1 = (arg0[3]+arg2) & ~arg2;`

Compiles to **5 instructions** matching target's instruction count
exactly. Only diff: target uses `v0` for the modified arg2 value,
mine uses `a2` (in-place modify):

```
target:                       built (arg2-- form):
addiu v0, a2, -1              addiu a2, a2, -1
not   t8, v0                  nor   t8, a2, zero
addu  t7, t6, v0              addu  t7, t6, a2
b     ...                     b     ...
 and  v1, t7, t8               and  v1, t7, t8
```

Pure register-allocation diff: a2 vs v0.

## CRITICAL FIX: pointer comparison must be UNSIGNED

`arg0` should be declared `u32 *` (or use `(u32)` casts on the
compare). The bounds check `if (arg0[2] < arg0[3])` is comparing
pointer values — `sltu` on the asm side. Declaring `s32 *arg0` produces
`slt` (signed) and breaks the match.

## Other variants tried (worse)

- `(arg0[3] + (align - 1)) & ~(align - 1)`: IDO peephole rewrites to
  `(arg0[3] + arg2) - 1`, inserting an extra `addiu reg, reg, -1`.
  Mask in t9 is computed but never used in the addu — IDO recomputes
  via a2 directly. 6 instructions, 1 extra.
- `mask = arg2 - 1; v1 = (arg0[3] + mask) & ~mask`: same as above.
- `mask = arg2 - 1; v1 = arg0[3] + mask; v1 &= ~mask`: same.
- Explicit `sum = arg0[3] + mask; v1 = sum & ~mask`: forced spill of
  sum to stack, BIGGER frame, worse.

The IDO peephole that turns `(arg0[3] + arg2 - 1)` into
`(arg0[3] + arg2) - 1` is stubborn — it kicks in unless `arg2` is
modified in place (the `arg2--` form).

## Permuter hint

Permuter on the **arg2-- form** (current seed). Just one register
permutation needed: `a2 → v0` for the in-place-modify. The rest of
the function already matches perfectly (with `u32 *arg0`).
