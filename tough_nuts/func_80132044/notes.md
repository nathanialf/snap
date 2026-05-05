# func_80132044 — read big-endian u32 from stream cursor

```c
u8 *p = (u8 *) arg0[2];
u32 b0 = *p++; arg0[2] = (s32)p;
u32 b1 = *p++; arg0[2] = (s32)p;
u32 b2 = *p++; arg0[2] = (s32)p;
u32 b3 = *p++; arg0[2] = (s32)p;
return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
```

Target uses a chain of fresh registers (t6 = a1+1, t9 = t6+1, t2 = t9+1, t4 = t2+1)
where each iter computes `next = current + 1` BEFORE reading the byte.

Mine in-place increments `p` (`addiu v1, v1, 1`) and reads `*p` after.
Same effective addresses, different instruction encodings.

## Tried

- `*p++` form: in-place `addiu v1, v1, 1`
- `*(p + N)` form: each access is `lbu reg, N(v1)` (offsets from base)
- Sequential `b0 = *p; p += 1;`: same in-place increment

## Permuter hint

Pure pointer-chain scheduling. Permuter should crack via reg-pinning.
Sister of func_8013200C (s16 reader, also parked). Cracking one cracks
both.
