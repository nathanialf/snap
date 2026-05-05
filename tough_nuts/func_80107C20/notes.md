# func_80107C20 — viewport setup with float scheduling

Sets up a Vp { vscale[4], vtrans[4] } from 4 float bounds (left,
top, right, bottom). Computes:
  - vscale[0] = (right - left) * 2
  - vscale[1] = (bottom - top) * 2
  - vscale[2] = 0x1FF (depth scale)
  - vtrans[0] = (left + right) * 2
  - vtrans[1] = (top + bottom) * 2
  - vtrans[2] = 0x1FF (depth trans)

## Diff

Body is correct in math and field writes, but float-op scheduling
diverges: the 0x1FF int-immediate setup (`addiu t8, $0, 0x1FF; sh
t8, 0xC(a0)`) lands AFTER the first vscale[0] sub-mul-trunc chain
in base, but BEFORE the chain in built.

Also: the `sh t8, 0xC; lh t9, 0xC; sh t9, 4` pattern (write _C,
read _C, write _4) is unusual — IDO is treating
vscale[2] = vtrans[2] = 0x1FF as a re-read instead of two parallel
sw-immediates. Trying `vp->vscale[2] = vp->vtrans[2]` matches that
pattern.

## Variants tried

- vscale[2] = vp->vtrans[2] (re-read after writing _C)

## Permuter hint

Pure float scheduling diff. May benefit from permuter to find a
local-decl or expression order that pins the 0x1FF setup later.
Could also try f32 args declared K&R (then permuter to undo the
double-promotion).
