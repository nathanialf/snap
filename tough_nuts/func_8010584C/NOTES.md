# func_8010584C — bump-allocate by 0x88 with two assertion guards

```c
idx = D_8004A910;
if (D_8004A828[idx] == 0) panic("first");
old = D_8004A830[idx];
if (old == D_8004A838[idx]) panic("buffer full");
D_8004A830[idx] = old + 0x88;
return old;
```

## Diff: register allocation

Two register-allocation differences:

1. Target uses **v0** for byte-offset (sll v0, v0, 2 in-place).
   Mine uses v1 (sll v1, v0, 2 — keeps idx in v0).

2. Target uses **v1** for `old` (lw v1, 0(a1); ...; or v0, v1, zero in jr delay).
   Mine uses a0 (lw a0, 0(a1); ...; move v0, a0).

Both produce identical semantics. Codegen is just different reg-alloc
choices. The `or v0, v1, zero` in jr's delay slot is a tell-tale sign
that target preferred v1 over v0 for the live-across-panic value.

## Try later

- Permuter on the natural form
- See if declaring `s32 idx = D_8004A910 * 4;` (byte-offset in source) coaxes v0 reuse
- Check if a forward declaration or function attribute alters reg-alloc
