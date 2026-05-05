# func_8012A640 — message-build wrapper with f32 + struct-array index

**ROM offset:** `0x2B240` **size:** `0x54` (21 instructions)
**VRAM:** `0x8012A640`

## Behaviour
Builds a message struct on stack, passes to `func_80029E80` with
queue at `(u8*)arg0 + 0x14`. The msg has:
- `type = 4`
- `ptr = (u8*)arg0[16] + arg0[15] * 48` (struct[index] entry, 0x30 stride)
- `fval = arg1` (the f32 arg)
- `flag = 0`

## What's matching
- `type = 4` store via `li $t6, 4; sh $t6`
- 48-byte stride via `sll/subu/sll` (`*4 - *1 = *3`, then `<<4 = *48`)
- `mtc1 $a1, $f12` (f32 arrives in $a1 because non-f first arg)
- `swc1 $f12, 0x28($sp)` final store

## Remaining diff (~10 instructions)
1. **Frame 0x30 vs 0x28**: base reserves 8 extra bytes between `$ra`
   (sp+0x14) and the msg (sp+0x20). Built has msg at sp+0x18 with
   no gap. Suspect the original msg struct is 24 bytes with leading
   8-byte padding, OR there's an unrelated 8-byte local also on
   stack. Adding a leading `s32 _pad0; s32 _pad1` to the struct
   fixed the frame size but the scheduling still diverges.
2. **Load order:** base loads `arg0[16]` (table base) FIRST, then
   `arg0[15]` (index). Built reverses, even when source has the
   `arg0[16] + arg0[15] * 48` expression with arg0[16] left-most.

## Variants tried
- `(s32*, f32)` ANSI prototype: matches register flow but loads in
  wrong order.
- K&R `f32 arg1`: f32→double promotion (per
  `feedback_snap_kr_float_promotion.md`) — totally diverges.
- Reordered statements (msg.ptr first vs msg.type first): different
  diff but still wrong.
- Struct with leading 2x s32 padding (Msg24 form): frame size matches.

## Permuter hint
Saved best-attempt is the Msg24 form (frame matches). Permuter
should try load-order knobs (e.g. introducing intermediate locals
for `arg0[16]` and `arg0[15]`).

The "8-byte gap between $ra and msg" pattern likely repeats across
this file's siblings — if cracked, document the trick.
