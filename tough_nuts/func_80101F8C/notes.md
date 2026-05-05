# func_80101F8C — list-walk caller (s1 save with no s1 use)

Walks list at D_80048730 calling func_80032630(item->unk_4, 1, 0) for
each, then func_80001D94 at end.

## Diff

Base saves s1 (sw s1, 0x18; lw s1, 0x18) but never uses it. Built
elides the save since s1 is unused, dropping frame from 0x20 to 0x18.

## Permuter hint

Need a way to force s1 to be saved. Maybe a dead local that takes
an s-register, or K&R declaration of an unused arg.
