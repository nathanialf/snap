# func_80100A64 — multi-list slot picker (frame size diff)

Tries to pick a Foo* from D_80048800[0..2] avoiding the values
returned by func_80035270 and func_8003D820. Sets D_8004880C and
related globals on success.

## Diff

Frame size: base uses 0x28, built uses 0x20. The 8-byte diff
cascades all the lui addresses. Body shape and call sequence look
correct in source.

## Permuter hint

Maybe an unused local needed to push frame size up. Or there's
extra padding from K&R-style spill of the unused arg0.
