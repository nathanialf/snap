# func_8010BCA8 — assertion + delay loop

**ROM offset:** `0xC8A8` **size:** `0x9C` (39 instructions)
**VRAM:** `0x8010BCA8`

## Behaviour
Asserts that `D_8004AC50->_1C->_1B8->_38 == 0` AND
`D_8004AC50->_1C->_1B8->_3C == 0xFEDCBA98` (magic value). On failure
calls panic via func_80025F24. Then loops `arg0` times: call
`func_80032630(&D_8004AC60, 1, 0)` followed by `func_80032780(0)`,
decrementing arg0 each iter.

```c
void func_8010BCA8(s32 arg0) {
    s32 *p = D_8004AC50->_1C->_1B8;
    if (p->_38 != 0 || p->_3C != 0xFEDCBA98) {
        func_80025F24(&D_80043720, *D_8004AC50->_18);
    }
    while (arg0 != 0) {
        func_80032630(&D_8004AC60, 1, 0);
        func_80032780(0);
        arg0--;
    }
}
```

## Remaining diff (regalloc)
- $v0 vs $v1 for D_8004AC50 base
- Different reg picks for the dereference chain

## Permuter hint
Pure regalloc. Permuter should crack quickly.
