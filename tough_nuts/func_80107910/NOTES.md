# func_80107910 — RGBA8888→RGBA5551 with passthrough mode

Color conversion: if `D_8004A954 == 3`, pass arg0 through; otherwise pack
top 5 bits of R/G/B + top 1 bit of A into RGBA5551 and replicate to upper
16 bits.

## Diff: register allocation in last 3 instructions

Target:

```
or  v0, t4, t6     ; v0 = result
sll t7, v0, 16
or  v1, t7, v0     ; v1 = (result<<16) | result
jr ra
 or v0, v1, zero    ; delay slot: v0 = v1
```

Mine:

```
or  v1, t4, t6     ; v1 = result
sll t7, v1, 16
or  v0, t7, v1     ; v0 = (result<<16) | result
jr ra
 nop
```

Same semantic; my version is even one instruction shorter. Target uses an
extra `move v0, v1` in jr's delay slot. To produce that, the compiler
needs to allocate the conversion result to v0 and the final |-shifted to
v1, then move v1 → v0.

Tried explicit `u32 final = (result<<16)|result; return final;` —
optimizer collapses `final` away.

## Why the redundant move?

IDO sometimes prefers to compute "result" into v0 (as if it were a
candidate return value), then later realizes the actual return is the
shifted-or form, computes that into v1, and moves v1 to v0 in the delay
slot. This happens when `result` and the final value are live on
different paths or when liveness analysis sees an early return that
returns something close to v0.

The early `if (D_8004A954 == 3) return arg0;` path returns arg0 in v0
directly. So the "second" path's result is the conversion. IDO might
preserve register conventions across both return paths in a way that
forces the intermediate move.

## Try later

- Permuter: scheduling/regalloc permutations
- Restructure as `u32 result = ...; u32 v = (result<<16)|result; return v;` and check
- Inline both returns: `return (D_8004A954 == 3) ? arg0 : ...;`
