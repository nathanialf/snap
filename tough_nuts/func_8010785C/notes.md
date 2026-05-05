# func_8010785C — align-up bump allocator with overflow trap

A bump-pointer allocator: aligns `arg0[3]` (current cursor) up to
`arg2` boundary if non-zero, advances by `arg1`, and traps if the
new cursor exceeds `arg0[2]` (max). On overflow it calls
`func_80025F24(D_80043260, arg0[0])` and spins in `while (1) {}`.

Returns the (pre-advance) aligned cursor.

## What's matching

Everything around the if/branch structure, the bump update, the
overflow check, and the diagnostic call site.

## What's different (CSE on `align - 1`)

Base computes `(align - 1)` once into v0, uses it both as the
addend (`arg0[3] + (align-1)`) and as the source for `~(align-1)`.

Built decomposes: computes `align - 1` for the mask, but turns the
addend into `(arg0[3] + align) - 1`, which inserts an extra
`addiu rN, rM, -1` instruction and shifts every subsequent slot.

```
8470  base 24c2ffff (addiu v0, a2, -1)   built 24d9ffff (addiu t9, a2, -1)
8474  base 0040c027 (not   t8, v0)       built 03204027 (nor   t0, t9, $0)
8478  base 01c27821 (addu  t7, t6, v0)   built 01c67821 (addu  t7, t6, a2)
847c  base 10000002 (b ...)              built 25f8ffff (addiu t8, t7, -1)
8480  base 01f81824 (and   v1, t7, t8)   built 10000002 (b)
8484  base ...                           built 03081824 (and v1, t8, t0)
```

## Variants tried

- `(arg0[3] + (align - 1)) & ~(align - 1)` — built decomposes
- `mask = align - 1; (arg0[3] + mask) & ~mask` — same diff
- Compound assigns `pos += mask; pos &= ~mask;` — different diff,
  same family

## Permuter hint

Try forcing CSE with a `register` or `volatile` local for the mask,
or split the if-body into two halves where the mask is reused via
pointer/load. May also clear at -O1 or by reordering the compare.
