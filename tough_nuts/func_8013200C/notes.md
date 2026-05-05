# func_8013200C — read big-endian s16 from a stream cursor

**ROM offset:** `0x32C0C` **size:** `0x38` (14 instructions)
**VRAM:** `0x8013200C`

## Behaviour
```c
u8 *p = (u8*)arg0[2];      // stream cursor
s16 v = (byte0 << 8) | byte1;  // big-endian 16-bit read
arg0[2] = p + 2;           // advance cursor
return v;
```

Per byte read, `arg0[2]` is updated immediately to `p+1` then `p+2`.

## What's matching
- Byte loads, shifts, ors, and sign-extension cast pattern correct.
- Two `arg0[2] = ...` updates flanking the second byte read.
- Frame-less function (no `addiu $sp` save).

## Remaining diff (~12 differing instructions)
1. **Register allocation:** base puts `p` in `$a1` and `byte0/v` in
   `$v1`. Built reverses: `p` in `$v1`, `byte0` in `$a1`.
2. **Final `arg0[2] = p+2` placement:** base puts the sw before jr
   (with the return-value sra in the delay slot). Built puts the sw
   in jr's delay slot.
3. **Fresh-temp vs in-place advance:** base materialises `t6 = p+1`
   and `t8 = p+2` as fresh registers. Built may reuse `$v1` for both.

## Variants tried
- `s16 v = *p++ << 8; arg0[2] = (s32)p; v |= *p++; arg0[2] = (s32)p;`
- `u8 b0 = *p; u8 *p1 = p+1; arg0[2] = (s32)p1; b1 = *p1;
   arg0[2] = (s32)(p1+1); return (b0 << 8) | b1;` (introduced p1)
- Pre-computed bytes b0, b1 in local s32 vars first, return last.

All variants left the same regalloc swap.

## Permuter hint
Permuter target. Cursor functions like this are a common pattern;
if it cracks, look for siblings (`func_80132044`, `func_80132094`
which is also a stream-decoder).
