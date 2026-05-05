# func_80115448 — five-store global init

Tiny init that sets D_8004B03C, D_8004B040, D_8004B044, D_8004B048, D_8004B04C
all to `&D_8004B050`. 11 instructions in target.

## Diff: lui-reuse pattern

Target does pairwise `$at` register reuse:

```
lui at, hi(D_8004B03C)       ; lui #1
sw  v0, lo(D_8004B03C)(at)
lui at, hi(D_8004B040)       ; lui #2
sw  v0, lo(D_8004B040)(at)
sw  v0, lo(D_8004B044)(at)   ; reuses lui #2
lui at, hi(D_8004B048)       ; lui #3
sw  v0, lo(D_8004B048)(at)
jr  ra
sw  v0, lo(D_8004B04C)(at)   ; reuses lui #3
```

So 3 luis, with B040+B044 sharing one and B048+B04C sharing another. Why
B03C is alone is unclear — same hi (0x8005) as the rest.

The pairwise sharing strongly implies adjacent-offset access from the same
base symbol. So source is probably:

```c
extern void *D_8004B03C;
extern void *D_8004B040[2];   /* covers B040, B044 */
extern void *D_8004B048[2];   /* covers B048, B04C */
```

But IDO `extern void *arr[2]; arr[0] = x; arr[1] = x;` HOISTS the array
base into a non-$at register first (`v1` or `a0`), then does both stores
with that base. Target uses `$at` (load-store-macro form) which IDO emits
when consecutive sw's haven't been hoisted yet.

## Tried

- 5 individual `extern void *` writes (each gets own lui — too many)
- Two `extern void *arr[2]` arrays (hoists base into v1/a0 — wrong reg + extra setup)
- Chained `D_8004B044 = D_8004B040 = p;` (separate luis per pair, double bases)
- Pointer-arith trick `*(&D_8004B040 + 1) = p;` (same as arr[1] hoist)

## Hypothesis

Need an idiom that emits two adjacent `sw v0, lo(sym1)(at); sw v0, lo(sym2)(at)`
where both reference the same `lui at, hi` — but where sym1 and sym2 are
separate symbols (so the hoisting doesn't kick in).

Maybe:
- Adjacent stores via a struct field by reference: `*(struct *)&D_8004B040 = ...; ((struct *)&D_8004B040)->second = ...;`
- A union over two distinct extern symbols
- Permuter scheduling pass on a struct-array form

The single solo lui for B03C is the easy part — it's the pair-sharing that
needs IDO's exact CSE mode, which `extern void *arr[2]` doesn't reproduce
because the hoist happens too eagerly.

## Try later

- decomp-permuter on the struct-array form
- IDO 5.3 (might be different optimizer behavior for $at vs hoisting)
