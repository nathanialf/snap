# func_8010EB28 + func_8010EB98 — switch-getter cluster (rodata layout issue)

Both are switch-on-arg1 getters with 9 cases returning `*(f32*)((u8*)arg0+OFF)`,
plus an empty case 10 (forces sltiu bound to 0xA).

**ROM offsets:**
- func_8010EB28: 0xF728 size 0x70
- func_8010EB98: 0xF798 size 0x70

## What's matching (16/17 instructions per func)
- `addiu t6, a1, -1; sltiu at, t6, 0xA; beqz at, default` (header)
- jumptable indirect jump
- 8 case bodies with `jr ra; lwc1 f0, OFF(a0)` pattern
- Last case (case 9) lwc1 falls through to shared default
- Default `jr ra; nop`

## Remaining diff: rodata jumptable placement

The .c emits its own rodata jumptable (anonymous compiler-generated)
at file offset that doesn't match the original ROM's
`jtbl_800438A8_main` (0x800438A8) and `jtbl_800438D0_main` (0x800438D0).
Result: ROM grows by 0x50 bytes and the lui/lo16 references in the
function bodies point to the wrong addresses (lui 0x8110 instead of
0x8004).

These symbols are declared in `config/undefined_syms_auto.us.txt` as
fixed addresses (0x800438A8, 0x800438D0), so the linker should bind to
those — but IDO emits its own anonymous jumptable that the compiler-
generated lui/lo references locally.

## Permuter hint / next steps

This needs splat-level rodata-subsegment configuration to extract the
existing jumptables from the baserom and merge them with the .c output:
- Add a [.rodata, c, func_8010EB28] subsegment at file offset
  matching vaddr 0x800438A8
- Or use `__attribute__((section(".rodata.<name>")))` on the jumptables
- Or hand-write the jumptables as inline asm

Pure C source mods can't fix this — it's a build-system gap.

Park indefinitely; revisit when splat rodata-handling is set up.
