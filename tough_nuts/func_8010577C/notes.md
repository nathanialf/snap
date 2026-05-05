# func_8010577C — alignment-checked queue init (a1 vs t8 reg choice)

Posts (8, 0x32, arg0, arg1) to a queue via func_800009A0(buf) then
asserts `((u32)&D_80048830 & 7) == 0`; if the global isn't 8-byte
aligned, traps via func_80025F24(D_800430A4) + spin.

## Diff (4/22 differ)

Body matches in shape; the address `&D_80048830` lands in `a1` in
base (lui+addiu a1) but in `t8` in built. Cascade: andi t9 vs andi
t8, beqz t9 vs beqz t8.

## Permuter hint

Try expressing the alignment check via a stored pointer that
flows through an a-reg, e.g. by passing it to a no-op function or
having it become a function arg form.
