# func_80101F24 — guarded init + 64-bit-arg call

Returns early if D_80048754 != 0 or D_8004874C == 0. Else inits
D_80048754 = D_8004874C, sets _8 = 2, and calls
func_8003CCA0(p->_50, (s64)p->_78).

## Diff

Frame: built 0x20 vs base 0x18. Mine adds extra spill space.
Cascades to all subsequent offsets.

## Permuter hint

The base uses a different reg/spill pattern. May need to drop the
intermediate `q` local and inline the references.
