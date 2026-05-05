# func_8012A5B0 — message-creator with byte arg + dispatch (sibling of func_801325B0)

**ROM offset:** `0x2B1B0` **size:** `0x84` (33 instructions)
**VRAM:** `0x8012A5B0`

## Behaviour
3-arg variant of `func_801325B0`. Adds an `unsigned char arg2`
spilled K&R, written to msg field at offset 0xC after the call.

```c
void func_8012A5B0(arg0, arg1, arg2)
s32 *arg0;
s32 *arg1;
unsigned char arg2;
{
    if (arg1[2] != 0) {
        s32 *r = (s32*) func_8002AB70(arg0, arg1);
        if (r != 0) {
            *(s16*)((u8*)r + 8) = 0xC;        /* type field */
            r[0] = 0;
            r[1] = arg0[7] + ((s32*)arg1[2])[0x36];
            *((u8*)r + 0xC) = arg2;
            arg1->_8->_C->func(3, r);
        }
    }
}
```

## Same wedge as `func_801325B0`
IDO emits `or $a3, $a1, $zero` BEFORE the beql (preserving arg1 in
`$a3` for post-call use). Base spills `$a1` directly via `sw $a1`
in the jal delay slot. This single instruction shifts the whole
function by 1 instruction.

## Sibling of `func_801325B0`
Same pattern, same wedge. **If one cracks, the other will too.**
Both should be revisited together.

## Permuter hint
The pre-call `or $a3, $a1, $zero` (or `or $a2, $a1, $zero` in 325B0)
is the wedge. Same investigation as 325B0.
