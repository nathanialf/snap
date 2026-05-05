# func_8010A344 — alloc + 16-field init with v1 split

K&R 4-arg constructor (arg2 is u8, bound-checked < 0x20). Calls
func_80008394() to alloc, then fills ~16 fields in the new struct
(mix of sw, sb, swc1, with arg0/arg1/arg3/arg2/0x21/0/0.0f/etc.).

## Diff

Body shape is correct but field-write SCHEDULING differs:
  - Base writes `_14 = arg1` BEFORE the trailing `_54 = 0; _58 = 0`
  - Built writes `_54 = 0; _58 = 0` BEFORE `_14 = arg1`
  - Base routes the trailing two stores through v1 (a copy of v0
    saved in the bnez delay slot), built uses v0

The bnez->delay-slot `v1 = v0` is the smoking gun: base preserves
the alloc result in BOTH v0 and v1, then uses v1 only for the
final 2 zero stores (probably to free v0 for some reason — but
v0 is unused otherwise).

## Variants tried

- ANSI struct + K&R u8 arg in the function signature
- Trap call with 3 args `func_80025F24(D_80043478, arg2, arg0)`

Both compile but the trailing scheduling doesn't match.

## Permuter hint

Try a separate `ret` local that aliases v at the end:
```c
ret = v;
ret->_54 = 0;
ret->_58 = 0;
return ret;
```
or split the field-write block into two scoped {} blocks to push
IDO into using v1 for the second block.
