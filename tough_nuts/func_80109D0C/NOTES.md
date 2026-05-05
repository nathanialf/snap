# func_80109D0C — struct init, returns 1

Initializer for a struct (lots of zero stores + 3 FP writes). Returns 1.

## Diff: v0=1 hoisted + reused as +4 offset

Target pattern at function entry:

```
addiu v0, zero, 1     ; v0 = return value, but ALSO used for offset
sll   t6, v0, 2       ; t6 = 4
sw    zero, 0x4C(a0)
... (other stores via a0 with normal offsets) ...
addu  v1, a0, t6      ; v1 = a0 + 4
sw    zero, 0x5C(v1)  ; → 0x60(a0)
sw    zero, 0x60(v1)  ; → 0x64(a0)
sw    zero, 0x64(v1)  ; → 0x68(a0)
sw    zero, 0x58(v1)  ; → 0x5C(a0)   (out of order)
... (more stores)
```

So IDO hoisted `v0 = 1` (the return value) to the top and re-used it via
`sll t6, v0, 2` to produce 4, then `v1 = a0 + 4`. The four stores via v1
are 0x60, 0x64, 0x68 (sequential), then 0x5C (backward). All offsets
were within s16 range so the v1 trick saves nothing — it's a quirk of
the source code.

Mine returns 1 too but emits `li v0, 1` near the end, no v1, no t6 trick.

## Hypothesis

Source likely has a struct field at 0x58 that's an array, and accesses
indices [1], [2], [3], [0] in that order:

```c
arg0->arr[1] = 0;   /* 0x5C */
arg0->arr[2] = 0;   /* 0x60 */
arg0->arr[3] = 0;   /* 0x64 */
arg0->arr[4] = 0;   /* 0x68 */
arg0->arr[0] = 0;   /* 0x58 — already covered separately */
```

But order doesn't match (last v1 write is 0x5C/index 1, not 0x58/index 0).

Or source uses a separate `s32 i = 1;` variable that the compiler keeps
in v0, and accesses `(&arg0->_58)[i+N]` with explicit pointer arithmetic.

## Try later

- Permuter on the natural form
- Struct with array at 0x58, with indexed stores in target's specific order
- A `s32 i = 1; arg0->arr[i+1]; arg0->arr[i+2]; arg0->arr[i+3]; arg0->arr[i];` pattern
- IDO-O1 to see if the optimization changes
