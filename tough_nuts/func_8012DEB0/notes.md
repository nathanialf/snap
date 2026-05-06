# func_8012DEB0 — message-builder with arg1 (s16)

**ROM offset:** `0x2EAB0` **size:** `0x58` (22 instructions)
**VRAM:** `0x8012DEB0`

## Behaviour
```c
EntryMsg msg = { type=3, entry_ptr=&entries[idx], value=arg1 };
func_80029E80((char*)arg0 + 0x14, &msg, 0);
```

Sibling of func_80132260 (already matched) and func_80132400 — same
3-field msg struct + dispatch via func_80029E80. Differs in that it
takes an `s16 arg1` value to embed in the msg.

## What's matching (~17/22)
- Frame size 0x30 (forced via `Frame { s32 _hole[2]; EntryMsg msg; }`
  to add 8 bytes of padding between ra and msg)
- All 22 instructions present, same data flow
- arg1 spilled K&R-style + re-read as halfword (`lh`) at sp+0x36

## Remaining diff (~5 instructions, scheduling-only)
Target schedules `lw entries (0x40)` BEFORE `addiu t6=3` and the
`sh type=3` store. Mine schedules:
- `addiu t6=3` hoisted to instr-4 (right after `addiu sp`)
- `lw idx (0x3C)` before `lw entries (0x40)`
- `sh type` immediately after spills, before any loads

## Variants tried
- ANSI proto: same diff
- K&R proto: same diff (uses K&R - this matches target's halfword spill)
- Reorder source statements (entry_ptr first, type last): same
- Cached `s32 *base = arg0[0x10]` local: bumps frame to 0x38 (worse)
- `Frame { hole; EntryMsg msg; }` struct wrapper: forces frame 0x30 ✓
- -O1: introduces extra arg0 stack-spill (worse)

## Permuter hint
Pure scheduling. The frame-size and 16-byte-msg layout is correct;
just need IDO to emit the operations in target's order:
1. spills
2. lw entries
3. addiu t6=3 + sh type
4. lw idx + lh arg1
5. sll/sub/sll for idx*0x30
6. addu + sw entry_ptr

Permuter could try: explicit access to arg0[0x10] via a register-
hinted local, dummy reads of unused fields, or rearranging the msg
struct field order.
