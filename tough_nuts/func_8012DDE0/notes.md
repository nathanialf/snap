# func_8012DDE0 — color-msg builder + send

```c
ColorMsg msg = { type=7, _8=0xFF, _9=0x51, _B=(arg1>>16)&0xFF, _C=(arg1>>8)&0xFF, _D=arg1&0xFF };
func_80029E80(arg0+0x4C, &msg, 0);
```

24 of 26 instructions match. Diff: target preserves arg1 in t3 (`move t3, a1`)
BEFORE writing the bytes, then `sb t3, 0x25(sp)` later. Mine emits
`sb a1, 0x25(sp)` directly while a1 is still arg1, then computes &msg
into a1 later.

Same end result; just whether IDO defers the byte-D write to use a saved
copy or writes early via a1. Tried `s32 saved = arg1` to force the
save — bumped frame to 0x30.

## Permuter hint

Pure scheduling. Permuter pin should crack quickly.
