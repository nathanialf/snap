# func_80123074 — guarded byte-set + dispatch

```c
if (D_800968C4[arg0] != -1) {
    D_800968C8[arg0] = arg1;
    func_80032440(D_800968E4, D_800968C4[arg0]);
}
```

## Diff: a2/a3 register swap (8 lines)

Target preserves arg0 in `a3`, masks arg1 byte into `a2`. Mine swaps —
arg0 in `a2`, arg1 byte in `a3`. Cascades through the byte store and
the second deref.

Tried K&R `u8 arg1`, ANSI `s32 arg1` with cast — both same swap.

## Permuter hint

Pure regalloc swap (a2 ↔ a3). Permuter pin should crack quickly.
