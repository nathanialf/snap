# func_8010FDA0 — three-byte conditional dispatch

**ROM offset:** `0x109A0` **size:** `0x68` (26 instructions)
**VRAM:** `0x8010FDA0`

## Behaviour
4-arg K&R function. For each non-zero byte arg, calls
`func_80009760(arg0, byte, 0)`.

```c
void func_8010FDA0(arg0, arg1, arg2, arg3)
s32 *arg0;
unsigned char arg1, arg2, arg3;
{
    if (arg1) func_80009760(arg0, arg1, 0);
    if (arg2) func_80009760(arg0, arg2, 0);
    if (arg3) func_80009760(arg0, arg3, 0);
}
```

## What's matching
- 4-arg K&R prologue with full home spills
- Three sequential `if-then-call` checks
- `or $a2, $zero, $zero` in jal delay (third arg = 0)

## Remaining diff
**arg1 is read via `andi $a1, $a1, 0xFF` (register) in base, but
built emits `lbu $t6, 0x1F($sp)` (memory) via spill.** arg2 and
arg3 both correctly go via lbu (from spill at +0x23 / +0x27). Just
arg1 doesn't.

The cascading shift this creates: built needs an extra
`or $a1, $t6, $zero` move before the jal (since the byte's in $t6
not $a1). That's 1 extra instruction, shifting everything.

## Variants tried
- All three K&R `unsigned char`: lbu for all three (wrong for arg1)
- arg1 `int` + `arg1 & 0xFF`: changes frame size (because `&arg2`
  somewhere bumps frame to 0x20)
- arg1 `unsigned char` + arg2/arg3 `int` with `((u8*)&arg)[3]`:
  same lbu/lbu/lbu pattern
- All `int` with explicit `& 0xFF` for arg1, byte-cast for arg2/arg3:
  frame bump (0x20) due to taking address

## Permuter hint
The trick is whether IDO emits andi or lbu for the FIRST byte arg.
Same K&R declaration produces both depending on... something. May
be a subtle source-side trigger: the order of statements, an
intermediate variable, or how `arg1` is referenced.
