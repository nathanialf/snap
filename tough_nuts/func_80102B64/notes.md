# func_80102B64 — multi-block conditional cleanup with v0 vs a2 reg choice

Four conditional cleanup blocks: each tests `arg0->_X - arg0->_Y` and
calls a different func with (start, length) args (or 5-arg call to
func_80002A10 in block 3).

## Diff

The body shape and call sequence match. The constant diff is which
register holds the early-loaded `arg0->_C` (and similar later
fields) value:
  - Base: into `v0`, then `or a0, v0, $0` to set up the call
  - Built: into `a2`, then `or a0, a2, $0`

This cascades through all four blocks because the same idiom
repeats. ~24 of 47 instructions differ only in rs/rt encoding.

## Variants tried

- Local `s32 d` for the difference value
- Inlined `arg0->_X - arg0->_Y` directly in the if and call args

Both produce identical reg choice (a2 instead of v0).

## Permuter hint

Try caching the field reads into separate named locals (especially
`s32 c = arg0->_C; s32 ten = arg0->_10;`) so IDO can colour them
into v0/v1 via dataflow rather than a-regs. May also need a
register hint to avoid the auto-promotion of "next call arg
preview" allocation.
