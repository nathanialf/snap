# func_80125A68 — varargs forwarder

Tiny varargs forwarder: takes (s32, ...) and calls func_80025938(s32, va_list).

## Diff

Target produces:

```
or    a1, sp, $zero
addiu a1, a1, 0x27
```

Mine produces:

```
addiu a1, sp, 0x27
```

Both compute `a1 = sp + 0x27 = (&arg0 + sizeof(s32) + 3)`, the input to the
`& -4` mask which yields `&arg0 + 4 = sp + 0x24`, i.e. the va_list anchor.

Result is identical. The target keeps the address build in two steps
(`sp` → `sp + 0x27`), mine folds. Tried:

- `va_start(ap, arg0)` then `_VA_ALIGN(ap, 4)`
- splitting into `char *base = (char*)&arg0; ap = base + sizeof(arg0); _VA_ALIGN(ap, 4);`
- declaring an unused `int ans;` to mirror libultra's osSyncPrintf

All fold to the single `addiu a1, sp, 0x27` form because IDO constant-folds
&arg0's stack offset, sizeof, and _VA_ALIGN's `+3` together.

## Hypothesis

Target codegen suggests the source has *some* form that prevents the fold —
maybe a `register` qualifier, an intermediate volatile, or a different
va_start expansion. Need to find a construct that forces IDO to emit:
1. compute sp into a1 (with `or` form, suggesting reg-to-reg move)
2. then offset by full constant 0x27

The unfolded sp→a1 step is what's hard to reproduce. May relate to varargs
being represented as a struct internally that requires its address taken
"raw" first.

## Try later

- K&R-style declaration: `void func_80125A68(arg0) s32 arg0; { ... }`
- Explicit register storage class on the va_list
- IDO 7.1 with -O1 (uniform with libultra) instead of -O2 game CFLAGS
- Look at decomp-permuter for a permutation that produces the 2-step form
