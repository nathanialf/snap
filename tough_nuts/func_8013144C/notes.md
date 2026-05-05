# func_8013144C — multu chain math (saturate-mix)

**ROM offset:** `0x3204C` **size:** `0xA0` (40 instructions)
**VRAM:** `0x8013144C`

## Behaviour
```c
v1 = (arg0[0x36] * arg0[0x33] * arg0[0x30]) >> 6;
e  = &arg1->entries[arg0[0x31]];
a2 = (e->byte_9 * arg1->hword_32 * arg1->hword_34) >> 14;
if (e->byte_A >= 0) {
    a2 = (u32)(e->byte_A * a2) >> 7;
}
v1 = (u32)(v1 * a2) >> 15;
return (s16) v1;
```

`bltz $a3, .L801314D4` with NOP delay slot → suggests an `if (s8 >= 0)
{ ... }` guard with no scheduling-coercible content. All multus are
unsigned (multu, not mult).

## What's matching
- Mathematical structure
- Number/order of multus
- Final `(s16)` cast via sll/sra pair

## Remaining diff (9 differing instructions)
**Load order:** base loads `arg0[0x36]`, `arg0[0x33]`, `arg0[0x30]`,
`arg0[0x31]` in that order. Built loads `0x30` first. So `multu` is
of `byte_30 * byte_36` instead of `byte_36 * byte_33`.

The original C source has v1's expression as `arg0[0x36] *
arg0[0x33] * arg0[0x30]` — left-to-right associative, byte_36 first.
But IDO scheduled differently regardless of source ordering and
parenthesisation.

## Variants tried
- Explicit parens: `((a * b) * c)` — same diff.
- Split into temp: `s32 t = a * b; v1 = (t * c) >> 6` — different
  load order, same number of diffs.
- Reordered `Entry *e` declaration before vs after v1 computation —
  small effect on a couple of instructions.

## Permuter hint
The first `multu` operand order is the wedge — once that picks the
right registers, the rest may fall in line. Permuter should try
swapping the order of byte loads and which subexpression goes to
the first multu.
