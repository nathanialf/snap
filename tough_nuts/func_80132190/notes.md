# func_80132190 — message-builder w/ entry_ptr + u8 value

Same family as func_8012DEB0 (parked) — type=2 instead of 3,
arg1 is u8 instead of s16. Same scheduling wedge:

Target order:
1. sw a1, 0x34(sp)    (arg1 spill FIRST)
2. lbu t7, 0x37(sp)    (byte reload)
3. sw ra, 0x14(sp)
4. lw v0, 0x40(a0)     (entries early)
5. addiu t6, 0, 2; sh t6, 0x20(sp)
6. lw idx, addiu &msg, ...

Built order: addiu type-const hoisted to instr-4, sw ra second,
lw idx before lw entries.

Same permuter target as func_8012DEB0. Cracking one likely cracks both.
