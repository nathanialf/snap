# func_80104910 — find-empty-slot then queue-on-overflow

Walks an array of 4 Slot[0x18] entries at D_80048AC8 looking for the
first slot with `_0 == 0`; if all 4 are full, calls
func_80032840(D_80048968, &i, 1) (a queue recv) which writes the slot
index, then recomputes p. Otherwise marks the found slot's _0 = 1.
Then writes p->_10 = arg0, p->_14 = arg1 and calls
func_80032630(D_80048918, &p->_4, 0).

## Diff

The base keeps `i` (loop counter) in `v1` (a register) throughout
the loop, spilling to sp+0x1C only ACROSS the func_80032840 call.
Built spills `i` to sp+0x18 from the very start because IDO sees
`&i` taken later and forces all-memory allocation for `i`.

Cascades to wrong frame offsets (0x18 vs 0x1C), wrong loop body
shape (extra lw/sw per iter), and 24+ instruction differences.

## Variants tried

- Plain `s32 i; ... &i ...` — full memory escape
- For-init / for-update reordering — same

## Permuter hint

The trick is partial-register-allocation: IDO must keep i in a
register across the loop and only spill it across the call. Try
declaring i in a tighter scope, or move the &i call into a helper
that takes a separate s32 by ref, then assign back.
