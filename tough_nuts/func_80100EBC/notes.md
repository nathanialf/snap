# func_80100EBC — 0x50-byte struct copy w/ a0/t6 vs t6/t7 reg choice

Copies a 0x50-byte block from D_800487A8 → D_80048758 (six 12-byte
unrolled iterations + an 8-byte tail), then calls func_80035740,
extracts bit 27 of D_8004882C as the arg to func_80035660, and
zeroes D_800487F8.

## Diff

The struct-copy loop body matches in shape (lw/sw stride, 0xC step),
but base routes:
  - DST (D_80048758) into `a0`, then `t9 = a0` for the running cursor
  - SRC (D_800487A8) into `t6` for the running cursor
Built routes:
  - SRC into `t7`
  - DST into `t6`

Different starting register choice cascades a 1-instruction
prologue swap and the loop's lw/sw register names.

## Variants tried

- `D_80048758 = D_800487A8;` struct assignment of `Block50`
- `bcopy(...)` — IDO doesn't inline (linker error)
- (memcpy not tried — would also need declaration)

## Permuter hint

Try driving the dst through a function-arg-shaped local
(e.g. `void *p = &D_80048758;`) so IDO chooses `a0` for the dst,
or wrap the copy in a static helper that takes (dst, src) and
gets inlined.
