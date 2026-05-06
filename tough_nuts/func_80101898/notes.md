# func_80101898 — auto-permuter score-0, but frame size diff

Auto-permuter found a "score 0" candidate using uninitialized
`Foo *new_var2` reads — synthetic permuter mutation. Built from that
seed compiles to frame -0x28; target frame is -0x20.

The permuter's score-0 metric counts mnemonic equivalence, not byte
equality. The 8-byte frame difference is the same plateau category as
func_80100A64.

## Permuter-found seed

In `runs/func_80101898/output-0-1/source.c`. Uses synthetic uninitialized
`Foo *new_var2` reads/writes that don't compile to the same byte layout
as target.

## Diff
Frame -0x28 (mine) vs -0x20 (target). All sp-relative offsets shifted +8.

## Verdict
Same plateau category as func_80100A64 — IDO needs to NOT allocate
8 extra bytes for the locals. Permuter pinned at score 0 but byte-mismatch.
Park indefinitely, like func_80100A64.
