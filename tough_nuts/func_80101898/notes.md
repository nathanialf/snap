# func_80101898 — sister of 80101810 with arg0 stack-vs-saved-reg twist

Calls `func_80036010(arg0)`, stores return at D_80048820, then runs
the same if/else dispatch as func_80101810 but the "if" branch only
fires when D_8004873C->_8 == 2 (extra check). End assigns
D_80048740 = arg0 (vs 80101810's D_8004873C = arg0).

## Diff

Base preserves arg0 across the func_80036010 call by spilling it to
the home position (sw a0, 0x20(sp)) in the call's delay slot, then
reloading via t-regs at each subsequent use. Frame size 0x20.

Built routes arg0 through s0 (callee-saved register), saving s0 in
the prologue and restoring at epilogue. Frame size grows to 0x28.

This decision is unforced by typical hints — neither K&R nor a
local-alias variable flips the choice.

## Variants tried

- ANSI C with `if (D_8004873C != 0 && D_8004873C->_8 == 2)`
- K&R with `void func_(arg0) Foo *arg0;`

Both give the same s0 codegen.

## Permuter hint

Try `register volatile`, or splitting the func_80036010 call into
its own statement, or rewriting the second `arg0->_8 = 3` to use a
stack-loaded version of arg0 explicitly. The key constraint to
satisfy: get IDO to spill arg0 to a stack home in the call's delay
slot rather than promote to s0.
