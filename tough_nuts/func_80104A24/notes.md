# func_80104A24 — sister of func_80104910 with mixed-width K&R args

5-arg variant (s32, u16, u16, s32, u16) of the slot-allocator pattern
in func_80104910. Same parking reason: the loop counter `i` is fully
spilled to memory because `&i` is taken by the queue call, but the
base keeps it in a register throughout the loop and only spills at
the call site. See `tough_nuts/func_80104910/notes.md` for the
underlying regalloc story.

## Permuter hint

Same as func_80104910 — needs partial-spill of `i`. Slot-array stride
is 0x24 (vs 0x18 in func_80104910) and the post-allocate write pattern
fills mixed s32 / s16 fields (_10 s32, _14 s16, _18 s32, _1C s32,
_20 s16). K&R declarations on the u16 args are needed for the lhu
read-from-spill pattern; that part appears correct.
