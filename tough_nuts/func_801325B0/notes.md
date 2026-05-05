# func_801325B0 — message-creator with function-pointer dispatch

**ROM offset:** `0x331B0` **size:** `0x78` (30 instructions)
**VRAM:** `0x801325B0`

## Behaviour
```c
void func_801325B0(arg0, arg1)
s32 *arg0;
A *arg1;
{
    if (arg1->_8 != 0) {
        s32 *r = (s32*) func_8002AB70(arg0, arg1);
        if (r != 0) {
            *(s16*)((u8*)r + 8) = 0xF;
            r[0] = 0;
            r[1] = arg0->_1C + arg1->_8->_D8;
            arg1->_8->_C->func(3, r);   /* function pointer at offset 8 of _C */
        }
    }
}
```

K&R signature with both args spilled. arg0 spilled in prologue,
arg1 spilled in jal delay slot.

## What's matching
- beql + jal + call structure
- Field offsets (_8, _1C, _D8, _C, etc.)
- Final indirect call with $a1=3, $a2=r

## Remaining diff (~25 instructions)
**Spurious `or $a2, $a1, $zero` before the beql** in built — IDO
decided to copy arg1 into $a2 before the call. Base doesn't do this.
Cascade-shifts everything by 1 instruction.

After the spurious move, built spills `$a2` (instead of `$a1`) into
the home position, then reloads `$a2` post-call. Logically
equivalent but byte-different.

## Variants tried
- With local `s32 *p = (s32*)arg1[2]`: same diff
- Without the local: same diff

## Permuter hint
The pre-call `or $a2, $a1, $zero` is the wedge. Once IDO can be
coerced to skip that, the rest may fall in line. Permuter should
try mutations on how arg1 is read in the body (e.g., reading via a
specific u32 cast, or moving reads earlier/later).
