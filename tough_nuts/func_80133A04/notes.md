# func_80133A04 — pop-from-list-head (tiny)

```c
Node *old = *arg0;
*arg0 = old->next;
return old;
```

4 instructions in target. Pure regalloc: target uses $t9 for the "next"
load. Mine picks $t6, $a1, $a2 depending on form. Permuter has score-10
plateaus.

## Permuter hint

Tiny function — just need t9 specifically. Permuter pin should work.
