# func_80109D0C — multi-field reset with weird base+4 indirection

**ROM offset:** `0xA90C` **size:** `0x64` (25 instructions)
**VRAM:** `0x80109D0C`

## Behaviour
Resets ~14 fields of arg0 (s32 zeros, byte zeros, plus 3 floats from
constants).

## What's tricky
Base computes `$v1 = $a0 + 4` and uses that as the base for the
middle 4 stores at offsets `+0x5C, +0x60, +0x64, +0x58` (which equal
`$a0 + 0x60, +0x64, +0x68, +0x5C`). The other stores use `$a0`
directly.

```
li   $v0, 1
sll  $t6, $v0, 2     ; t6 = 4
...
addu $v1, $a0, $t6   ; v1 = arg0 + 4
sw   $0, 0x5C($v1)   ; *(arg0+0x60) = 0
sw   $0, 0x60($v1)   ; *(arg0+0x64) = 0
sw   $0, 0x64($v1)   ; *(arg0+0x68) = 0
sw   $0, 0x58($v1)   ; *(arg0+0x5C) = 0
```

Note the order: 0x60, 0x64, 0x68, then 0x5C (out-of-order, last).

## Variants tried
- Direct `arg0[N] = 0` for all: built doesn't compute the +4 base
- Explicit `s32 *p = arg0 + 1; p[N] = 0`: built constant-folded the +1
  away
- `register s32 i; i = 1; p = arg0 + i`: same fold

## Permuter hint
The base-pointer indirection is hard to coerce. The original source
likely uses something that prevents IDO from constant-folding. Try:
- A volatile compute that produces 1
- An array-index expression like `arg0[1].field` if arg0 is an
  array of 4-byte structs
- The function may be inlining a memset/setfield helper that has
  inherent base+4 indirection
