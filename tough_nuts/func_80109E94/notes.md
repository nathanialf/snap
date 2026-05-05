# func_80109E94 — append node to child-list

**ROM offset:** `0xAA94` **size:** `0x98` (38 instructions)
**VRAM:** `0x80109E94`

## Behaviour
Sibling of func_80109E20. Allocate a new Node via `func_80008A30(arg0)`,
append to the child-list at `arg0->_10`. Walk via `_8` (prev) field
to the end before linking.

```c
Node *func_80109E94(Node *arg0, void *arg1) {
    Node *n = func_80008A30(arg0);
    Node *cur = arg0->_10;
    if (cur != 0) {
        while (cur->prev != 0) cur = cur->prev;
        cur->prev = n;
        n->next = cur;
    } else {
        arg0->_10 = n;
        n->next = 0;
    }
    n->_14 = arg0;
    n->_10 = 0;
    n->prev = 0;
    n->_4 = arg0->_4;
    n->_50 = (s32)arg1;
    func_80009D0C(n);
    return n;
}
```

## What's matching (when grouped with 80109E20)
- jal func_80008A30 + delay sw $a0
- bnel/beql for the cur != 0 / cur == 0 dispatch
- Walk-to-end loop with bnel + lw in delay (peek-ahead idiom)
- Final field-init sequence (with proper interleaving from sibling 80109E20 lesson)

## Remaining diff (regalloc swap + scheduling)
**$a0 vs $a2 for cur** — base allocates cur to $a2, built picks $a0.
After my source's `if (cur != 0)` inversion to fix the beql direction,
the regalloc still doesn't match (different choice of which arg-reg to
hold the cur pointer).

## Variants tried
- `if (cur == 0) { ... } else { ... }`: bnel emitted (wrong)
- `if (cur != 0) { ... } else { ... }`: beql emitted (right) but
  regalloc still off

## Permuter hint
Pure regalloc once the if-condition is `!= 0` instead of `== 0`.
Permuter should crack quickly. May benefit from the same field-write
ordering trick that worked on the matched func_80109E20 sibling
(`n->_14` before `n->_10`).
