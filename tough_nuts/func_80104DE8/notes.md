# func_80104DE8 — switch dispatch with case-0 / case-1 setup

**ROM offset:** `0x59E8` **size:** `0x94` (37 instructions)
**VRAM:** `0x80104DE8`

## Behaviour
```c
void func_80104DE8(s32 *arg0) {
    switch (arg0[4]) {
        case 0:
            func_8003B780(&D_800488D0, arg0[3], 0x600, D_80049730);
            arg0[5] = D_8004974F;
            break;
        case 1:
            D_80049730[0x1F] = arg0[5];
            func_8003B530(&D_800488D0, arg0[3], 0x600, D_80049730, 1);
            break;
    }
}
```

## What's matching (32/37 instructions)
- Switch dispatch shape via `lw $v0, 0x10($a0); beqz/beq $v0, $at`
- Case 0 path: `func_8003B780` call + `arg0->_14 = D_8004974F` (lbu)
- Case 1 path: `D_80049730[0x1F] = (u8)arg0->_14` + `func_8003B530`
- Default-empty break

## Remaining diff (5 instructions)
Pure scheduling: order of `lui $a0, hi(D_800488D0)`, `lui $a3,
hi(D_80049730)`, and `or $s0, $a0, $zero` differs in the prologue
(IDO interleaves pre-loaded constants differently).

## Variants tried
- if/else if cascade: emitted bnez instead of beqz — totally
  different layout (8 diffs)
- switch (canonical form): 5 diffs (saved as seed)

## Permuter hint
Pure prologue scheduling. Permuter should crack quickly. May need
intermediate temps to reorder lui materialization.
