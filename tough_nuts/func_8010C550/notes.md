# func_8010C550 — tree-walk fallback returns

**ROM offset:** `0xD150` **size:** `0x64` (25 instructions)
**VRAM:** `0x8010C550`

## Behaviour
```c
void *func_8010C550(s32 *arg0) {
    if (arg0->_10 != 0) return arg0->_10;
    if (arg0->_8 != 0) return arg0->_8;
    while (1) {
        s32 *v = (s32*)arg0->_14;
        if ((s32)v == 1) return 0;       /* sentinel */
        if (v->_8 != 0) return v->_8;
        arg0 = v;
    }
}
```

Walks `arg0->_14` chain looking for the first node with non-null `_8`.
Returns `arg0->_10` or `arg0->_8` if either is non-null first.
The sentinel value `1` (treated as a pointer) ends the walk.

## What's matching
- All 25 instruction shapes correct
- beql + bnel patterns for the early-return fallbacks
- Backward `b` to loop top with `or $a0, $v0, $zero` in delay (advance arg0)

## Remaining diff (9/25 instructions)
**$v0 vs $v1 register-allocation swap throughout.** Base loads
arg0->_10 directly into $v0 (return register), so `return v` is
trivially `jr $ra`. Built loads into $v1, then needs an extra
`or $v0, $v1, $zero` before the jr.

## Variants tried
- Direct `return arg0[N]` form: regalloc swap
- `void *r = arg0[N]; if (r) return r;` temp form: same swap
- Goto-loop form: same swap

## Permuter hint
Pure $v0/$v1 regalloc. Should crack quickly. The wedge appears to be
which register IDO picks for the first read — once $v0 is the choice,
the rest cascades correctly.
