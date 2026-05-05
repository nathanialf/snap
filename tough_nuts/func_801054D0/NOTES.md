# func_801054D0 — copy 16-byte slot then call hook

`D_8004A8B8 = D_8004A928[D_8004A910]; func_80007850(&D_8004A8B8);`

## Diff: load-all then store-all vs interleaved

Target loads all four words first, then stores all four:

```
lw t9, 0(v0); lw t0, 4(v0); lw t1, 8(v0); lw t2, c(v0)
sw t9, 0(a0); sw t0, 4(a0); sw t1, 8(a0); sw t2, c(a0)  (last in delay slot of jal)
```

Mine interleaves load+store:

```
lw t1, 0(v0); sw t1, 0(a0)
lw t0, 4(v0); sw t0, 4(a0)
lw t9, 8(v0); sw t9, 8(a0)
lw t0, c(v0); sw t0, c(a0)  (last in delay slot)
```

Same instruction count, different order and different register count
(target uses 4 distinct regs t9/t0/t1/t2; mine reuses 2).

Tried `Slot *src = &arr[i]; D_8004A8B8 = *src;` — same interleaving.

## Hypothesis

IDO emits the load-all-then-store-all pattern only for certain struct
copy contexts. Maybe:

- The source struct is declared in a way that triggers IDO's "memcpy
  inline" rather than its "field-by-field copy"
- Could need a fixed-size memcpy() call which IDO inlines
- Or struct copy via assignment of a local that's been read first

## Try later

- `Slot src; src = arr[i]; D_8004A8B8 = src;` (extra local copy)
- `bcopy(&arr[i], &D_8004A8B8, sizeof(Slot));`
- decomp-permuter scheduling
