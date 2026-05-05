# func_801289C0 — minimal msg-build wrapper (sibling of func_8012A640 family)

**ROM offset:** `0x295C0` **size:** `0x4C` (19 instructions)
**VRAM:** `0x801289C0`

## Behaviour
1-arg variant of the func_8012A640 family. Sets msg.type=1, computes
msg.ptr from arg0->_40[arg0->_3C] (48-byte stride), sends.

```c
void func_801289C0(s32 *arg0) {
    Msg msg;
    msg.type = 1;
    msg.ptr = (s32)((u8*)arg0[16] + arg0[15]*48);
    func_80029E80((u8*)arg0 + 0x14, &msg, 0);
}
```

## Same wedge as func_8012A640 family
- Frame 0x30 in base (msg at sp+0x20, 8-byte gap between $ra and msg)
- Built emits frame 0x28 (msg at sp+0x18)
- Adding leading 8-byte padding to the struct fixes frame size but
  scheduling/regalloc still diverges (~11 instruction diffs)

## Variants tried
- 16-byte Msg (5 fields × 4 bytes): frame 0x28, wrong size
- 24-byte Msg with leading `s32 _pad0; s32 _pad1`: frame 0x30 matches
  but ~11 scheduling diffs in body
- `s64 _slack` local: optimized away

## Permuter hint
**Same family as func_8012A640, func_8012DEB0, func_80132190,
func_80132260** — they all share this 0x30-vs-0x28 frame quirk.
Cracking one cracks the family. Likely needs a specific local-decl
shape that survives DCE and forces the 8-byte gap.

Hypothesis: original source uses something like `OSMesg msg[N]` or
similar typed local that has 8-byte alignment requirement (unlike
the int-only Msg struct here). Worth trying with libultra OSMesg
typedefs if/when the segment-rodata problem gets unblocked enough
to import them.
