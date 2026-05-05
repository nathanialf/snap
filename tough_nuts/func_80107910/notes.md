# func_80107910 — bit-pack with v0↔v1 regalloc twist

Tiny leaf bit-pack that converts a packed 32-bit color to a 16-bit
field duplicated to upper 16 bits. Body matches except the final 4
instructions: base computes the bit-pack into v0, then `(v0 << 16) | v0`
into v1, then copies `v1 → v0` in the jr-delay slot. Built code computes
the bit-pack into v1, then `(v1 << 16) | v1` into v0 directly, with a
nop in the delay slot — saving a register move.

## Diff (built vs base, last 4 instructions)

```
00008550  base 018e1025 (or v0,t4,t6)   built 018e1825 (or v1,t4,t6)
00008554  base 00027c00 (sll t7,v0,16)  built 00037c00 (sll t7,v1,16)
00008558  base 01e21825 (or v1,t7,v0)   built 01e31025 (or v0,t7,v1)
00008560  base 00601025 (or v0,v1,$0)   built 00000000 (nop)
```

## Variants tried

- Single var: `v0 = combined; return (v0 << 16) | v0;` — built version
- Two vars: `v0 = combined; v1 = (v0 << 16) | v0; return v1;` — built version
- Self-assign: `v0 = combined; v0 = (v0 << 16) | v0; return v0;` — built version

All three produce the same regalloc (compute in v1, return in v0). The
base preference for "compute in v0, copy to v1, copy back" is an
unforced choice the compiler keeps making against; permuter may need
to flip a non-obvious local declaration order or split the dup-shift.

## Permuter hint

Try splitting `(v0 << 16) | v0` into two statements:
```c
v1 = v0 << 16;
v1 |= v0;
return v1;
```
or introduce a dummy variable that pins v0 (e.g. `v0 = combined; (void)v0;`)
to force the compiler to keep `combined` in v0.
