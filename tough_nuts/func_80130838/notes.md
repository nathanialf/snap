# func_80130838 — switch dispatcher with msg-struct case

**ROM offset:** `0x31438` **size:** `0xB4` (45 instructions)
**VRAM:** `0x80130838`

## Behaviour
Reads `arg0[6]` as a node pointer; if non-NULL, calls `func_80031B90`
which fills out `msg.type`. Switches on type:
- case 1: func_800308EC + func_8013075C
- case 3: func_80131188 + func_8013075C
- case 4: arg0[11]=2, msg.type=0x10, send to queue at arg0+0x4C

## What's matching
- Stack frame 0x38 bytes (16-byte ScreenMsg at sp+0x28)
- All call sites + arg setup
- `arg0[11] = 2` hoisted into case-4 branch's delay slot via the
  16-byte struct ensures the right `sw $t6, 0x2c($s0)` placement.

## Remaining diff (8 differing instructions, 32 of 45 unchanged)
1. **beql vs beq** for the null check `if (p != 0)`. Base emits
   `beql $a2, $zero, end` with `lw $ra` in delay slot (only on taken
   — the epilogue's first instr). Built emits `beq` with the move
   `or $a0, $a2, $zero` in delay slot.
2. **`addiu $a1, $sp, 0x28` hoisted before the branch** in base
   (filling a prologue slot), then re-computed at 0x31468 for case 4.
   Built emits the addiu in the JAL delay slot of `func_80031B90`.

## Variants tried
- `if (p == 0) return;` early-return form: same diff.
- `register s32 *p`: no change.
- `volatile ScreenMsg msg`: no change.
- Local `s16 *typep = &msg.type`: bumped frame to 0x40 (worse).

## Permuter hint
Pure scheduler-level diff: the addiu hoist forces beql with a
no-useful-instr delay slot. Likely needs the permuter's `if (1)`
synthetic guards to coax IDO into emitting beql. Or there's an
unidentified source-side knob in how `&msg.type` is materialised
that triggers the early hoist.
