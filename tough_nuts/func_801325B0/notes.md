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
            vt = arg1->_8->_C;
            ((void(*)(s32*, s32, s32*)) vt[2])(vt, 3, r);  /* 3-arg call: this + 3 + r */
        }
    }
}
```

The dispatch is a 3-arg call: `vt = arg1->_8->_C` is the "this" pointer,
which is also where the function pointer is read from at offset 8.

## What's matching (~25/30)
- 3-arg call signature: a0=vt (this), a1=3, a2=r
- Single dereference for fp lookup (NOT pointer-to-fp)
- Field offsets and arithmetic
- arg1 reload into $a3 post-call (target re-uses $a3 as base)

## Remaining diff (~3 instructions)
**Spurious `or $a3, $a1, $zero` before the beql** in built — IDO copies
arg1 into $a3 BEFORE the jal so it survives, then spills $a3. Target
spills $a1 directly in jal delay slot (`sw $a1, 0x1C(sp)`) and reloads
to $a3 after the call — same end state, 1 fewer instruction.

## Variants tried
- ANSI prototype + 3-arg call: closest (only early-move diff)
- K&R prototype: same diff
- With/without local `vt`: same diff

## Permuter hint
Pure scheduling wedge — single `or a3, a1, 0` before beql. Permuter
should try mutations on the order of `arg1[2]` reads or introducing
dummy uses of arg1 to defer IDO's pre-spill copy decision.
