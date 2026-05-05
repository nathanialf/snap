# func_80132094 — variable-length integer decoder

**ROM offset:** `0x32C94` **size:** `0x4C` (19 instructions)
**VRAM:** `0x80132094`

## Behaviour
Read bytes from the stream at `arg0[2]`. Each byte's high bit (0x80)
indicates "more to come". Lower 7 bits per byte; running accumulator
shifts left by 7 each iteration.

```c
b = *p++;
if (!(b & 0x80)) return b;
v = b & 0x7F;
do {
    b = *p++;
    v = (v << 7) + (b & 0x7F);
} while (b & 0x80);
return v;
```

## What's matching
- Outer-if + do-while structure
- bnez at loop bottom (regular bne, not bnel — single-byte advance
  is in body, not delay slot)
- Final `or $v0, $a1, $zero` to set return reg

## Remaining diff (18/19 instructions)
**Pure register-allocation swap.** Base puts `p` in `$v1`, `byte` in
`$v0`, `v` (accumulator) in `$a1`. Built puts `p` in `$v0`, `byte` in
`$v1`. The function flow and instructions are otherwise identical.

The high-bit-fall-through delay slot pattern (`or $a1, $v0, $zero`)
in beqz delay is "common write hoisted into delay slot" — that part
matches.

## Variants tried
- Reordered local declarations
- Used post-increment vs pre-increment vs explicit `p = p+1`

## Permuter hint
Very high regalloc-only ratio (18/19 instructions diff). Permuter
should crack quickly. After cracking, document why IDO chose `$v1`
vs `$v0` for the pointer — if there's a source-side trigger, save
to memory.
