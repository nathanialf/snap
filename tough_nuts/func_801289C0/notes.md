# func_801289C0 — small msg builder + send (struct layout)

Builds a stack message {type=1 (s16), ptr = arg0->_40 + arg0->_3C * 48}
and calls func_80029E80(&arg0->_14, &msg, 0).

## Diff

Body shape correct. Struct layout: base places msg at sp+0x20 with
frame 0x30, leaving sp+0x18..0x1F as padding. Built places msg at
sp+0x18 with frame 0x30 (when struct is sized to 24 bytes) — wrong
slot. Mismatched offset cascades to the addiu a0,sp,0x20 instruction.

## Permuter hint

Need a way to push the struct to the higher-offset half of the
local area. Maybe add another local that demands the higher slot
or use an explicit volatile to force ordering.
