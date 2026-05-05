# func_8012DF10 — message-creator with signed-byte abs() (sibling of func_801325B0)

**ROM offset:** `0x2EB10` **size:** `0x94` (37 instructions)
**VRAM:** `0x8012DF10`

## Behaviour
Same shape as func_801325B0 / func_8012A5B0 family but with:
- `type = 0x10`
- arg2 is `signed char` and stored as abs value to msg+0xC (uses
  bgez+negu for the absolute value)

```c
void func_8012DF10(arg0, arg1, arg2)
s32 *arg0;
s32 *arg1;
signed char arg2;
{
    if (arg1[2] != 0) {
        s32 *r = func_8002AB70(arg0, arg1);
        if (r != 0) {
            *(s16*)((u8*)r+8) = 0x10;
            r[1] = arg0[7] + ((s32*)arg1[2])[0x36];
            r[0] = 0;
            *(s32*)((u8*)r+0xC) = (arg2 < 0) ? -arg2 : arg2;
            arg1->_8->_C->func(3, r);
        }
    }
}
```

## Same wedge as func_801325B0 family
- IDO routes arg1 through different reg
- `signed char` causes sll/sra sign-extension dance at function start
  (plus the spill spilled the signed value)

## Permuter hint
Same wedge as func_801325B0 / func_8012A5B0. Cracking one cracks the
family. Plus the abs() handling needs to match.
