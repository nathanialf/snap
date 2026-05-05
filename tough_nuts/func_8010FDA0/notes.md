# func_8010FDA0 — 4-arg K&R u8 dispatcher (frame size 0x20 vs 0x18)

For each non-zero u8 (arg1, arg2, arg3), call
func_80009760(arg0, byte, 0).

## Diff

Mine emits a frame of 0x20 with home spills at sp+0x20..0x2F PLUS
an extra spill of the masked a1 at sp+0x1C. Base uses frame 0x18
with home spills at sp+0x18..0x27 (caller's home positions) and
only a single spill of the unmasked a1 at sp+0x1C BEFORE andi.

The K&R `u8 arg1` declaration triggers an extra "masked-register
copy spill" in built that base doesn't have.

## Permuter hint

Try ANSI declaration with explicit `(u8) arg1` casts. Or try having
arg1 as s32 in K&R while arg2/arg3 stay u8.
