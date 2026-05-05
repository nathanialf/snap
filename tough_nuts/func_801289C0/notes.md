# func_801289C0 — small msg builder + send (struct slot cracked, scheduling pending)

Builds a stack message {type=1 (s16), ptr = arg0->_40 + arg0->_3C * 48}
and calls func_80029E80(&arg0->_14, &msg, 0).

## Slot offset — RESOLVED

Adding a trailing dummy `s32 _filler[2];` AFTER the Msg local pushes
msg to sp+0x20 (matching base). Without filler, msg lands at sp+0x18.
The filler is preserved by IDO since it's a stack-allocated array of
non-trivial size (not a scalar that gets dead-coded).

```c
typedef struct Msg {
    s16 type;
    s16 pad;
    u8 *ptr;
    s32 _8;
    s32 _C;
} Msg;  // 16 bytes

void func_801289C0(Foo *arg0) {
    Msg msg;
    s32 _filler[2];   // KEEP — pushes msg to sp+0x20
    ...
}
```

This is a generalization of the saved memory rule "smaller locals
declared first; IDO -O2 places larger at higher offset" — here the
trailing array is the "smaller filler" forcing the larger-named
struct earlier.

## Remaining diff — prologue scheduling

Built emits `addiu t6, $0, 1` BEFORE `sw ra` and the `sh t6, 0x20(sp)`
(msg.type=1 store) is scheduled earlier in the prologue than base.
Base order: `sw ra; lw v0, 0x40(a0); addiu t6, 1; sh t6, 0x20(sp);
...`. ~5–8 prologue instructions differ in order even though all
values are correct.

## Permuter hint

Pure prologue scheduling diff. Permuter should crack with the
seed using the `Msg msg; s32 _filler[2];` decl order.
