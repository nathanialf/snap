# func_8010A2B0 — multi-list cleanup with array+linked-list walk

**ROM offset:** `0xAEB0` **size:** `0x94` (37 instructions)
**VRAM:** `0x8010A2B0`

## Behaviour
Cleanup function:
1. Reset arg0->_4->byte_F = 0 and arg0->_4->_48 = 0
2. Walk a 2-pointer array at arg0[0x64..0x68], call func_8000888C on each non-null
3. Walk linked list at arg0->_6C, call func_80008960 on each node
4. Call func_80008BFC(arg0)

```c
void func_8010A2B0(s32 *arg0) {
    *(u8*)((u8*)arg0[1] + 0xF) = 0;
    *(s32*)((u8*)arg0[1] + 0x48) = 0;
    /* arr-of-2 walk via cur += 4 with i < 8 */
    /* linked list walk via node = next */
    func_80008BFC(arg0);
}
```

## Remaining diff (~25/37 instructions)
Pure regalloc + scheduling. Saved register allocation differs:
- base uses $s0/$s1/$s2 with specific spill positions
- built uses $s0/$s1/$s2/$s3 with shifted spills

The for-loop counter increment (`i += 4`) is in the bne delay slot
in base; in built it's in different position.

## Permuter hint
Heavy regalloc. Permuter should crack with mutations on the loop
structure (for-loop vs while, explicit s32 i declared first vs last).
