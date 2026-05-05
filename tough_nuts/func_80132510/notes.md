# func_80132510 — find empty slot in Entry array, init it

**ROM offset:** `0x33110` **size:** `0x90` (36 instructions)
**VRAM:** `0x80132510`

## Behaviour
Linear search through `arg0->_40[0..arg0->_44]` (48-byte stride
entries). Finds first slot where `e->_1C == 0`, initializes with
arg1 plus magic constants and a scaled `arg1->_D * 0x7FFF >> 7`
formula, returns slot index. Returns -1 if no slot or array empty.

```c
s32 func_80132510(s32 *arg0, u8 *arg1) {
    s32 cap = arg0->_44;
    Entry *base = (Entry*)arg0->_40;
    s16 i;
    if (cap > 0) {
        for (i = 0; i < cap; i++) {
            Entry *e = &base[i];
            if (e->_1C == 0) {
                e->_1C = arg1;
                e->_20 = 5;
                e->_28 = 0;
                e->_2E = 0x40; e->_2F = 0;
                e->_24 = 1.0f;
                e->_2C = (arg1[0xD] * 0x7FFF) >> 7;
                return i;
            }
        }
    }
    return -1;
}
```

## What's matching
- multu $v1, 0x30 stride computation
- 1.0f via `lui 0x3F80; mtc1`
- `arg1[0xD] * 0x7FFF >> 7` via `sll, subu, sra` chain (no actual mul)
- Loop sign-extension via `sll 16; sra 16`
- Final return via `or $v0, $v1, $zero`

## Remaining diff
**Spurious `or $a2, $a1, $zero` before the loop** — IDO preserves
arg1 in $a2 across the loop iterations (because arg1 is used at the
end inside the if-block for the e->_2C calc). Base does NOT preserve
arg1 — it keeps reloading from $a1 directly within the loop body.

This is the same wedge as the func_801325B0 family — arg1 routing
through a temp register early instead of being used directly.

## Permuter hint
Similar wedge to func_801325B0 family. Permuter should try
restructuring the e->_2C computation: extract the byte read earlier,
or use a different access pattern that doesn't force arg1 preservation.
