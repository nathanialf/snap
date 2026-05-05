# func_80107964 — `or v1, a0, zero` preserve copy + tight scheduling

Stores three int args to globals at D_8004A968+0/4/8, then to a stack
buffer at sp+0x18 (with constants 5 and 0x64 at the head and the args
trailing at +0x24/+0x28/+0x2C), then calls func_800009A0(sp+0x18).

Body matches structurally but the base inserts an extra
`or v1, a0, zero` early to keep arg0 alive across some scheduling
window, so the later `sw arg0, 0x3C(sp)` becomes `sw v1, 0x3C(sp)`.

Built code skips the copy and stores `a0` directly to the stack —
saving an instruction but yielding a different total instruction
count, which cascades the rest of the prologue into a different
schedule.

## Diff trail (best variant so far, simple sequential stores)

```
0000857C  base ac440000 (sw a0,(v0))     built afa4003c (sw a0,0x3c(sp))
00008580  base 00801825 (or v1,a0,$0)    built ac440000 (sw a0,(v0))
00008584  base afae0018                  built afae0018      (= same)
...
000085A4  base afa60044 (sw a2,0x44)     built 8fbf0014 (lw ra)
```

12 of 21 instructions in different positions; values match modulo
the missing `or v1, a0, zero` slot.

## Variants tried

- Sequential global-then-stack stores
- Stack-then-global stores
- Intermediate `s32 t = arg0;` to coax a copy — only enlarged frame
- (Add more if attempted)

## Permuter hint

- Try defining a local pointer `s32 *p = sp18;` and writing the call
  arg as that pointer, to force `a0 = sp + 0x18` to dominate arg0
  liveness.
- Try adding a no-op use of arg0 between the global writes (e.g.
  `t = arg0 | 0;`) so IDO chooses to keep arg0 in v1.
- Try declaring arg list K&R-style (forces `sw a0,...` spill at
  prologue — though base has no such spill, this may still flip
  the regalloc pattern).
