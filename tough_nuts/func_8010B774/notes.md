# func_8010B774 — list-tail-insert with default-arg substitution

**ROM offset:** `0xC374` **size:** `0x94` (37 instructions)
**VRAM:** `0x8010B774`

## Behaviour
3-arg list-tail-insert. arg0 and arg2 default to `D_8004AC44` if NULL.
Allocates a new node via `func_8000B6F0(arg0)`, fills fields,
appends to arg0's list (head at +0x3C, tail at +0x40, count at +0x44).

```c
s32 func_8010B774(void *arg0, void *arg1, void *arg2) {
    Node *n;
    if (arg0 == 0) arg0 = D_8004AC44;
    if (arg2 == 0) arg2 = D_8004AC44;
    n = func_8000B6F0(arg0);
    n->field_4 = arg2;
    n->field_8 = arg1;
    if (head == 0) {
        tail = n;
        head = n;     /* tail=n hoisted into both branches */
    } else {
        tail->next = n;
        tail = n;
    }
    n->next = 0;
    count++;
    return 0;
}
```

## What's matching
- Two `if (arg == 0) arg = default` substitutions (bnez + bnel
  patterns)
- jal func_8000B6F0
- if-else with hoisted `tail = n` in both branches (the only way
  to match base's structure — the canonical "tail = n; if/else"
  factoring DOESN'T work)

## Remaining diff (5/37 instructions)
Pure register-allocation diff in the post-call reload sequence:
base reloads arg0 then arg2 then arg1; built reloads arg2 then
arg0 then arg1. Both produce the same value flow but pick
different `lw` orderings.

## Variants tried
- ANSI prototype: same diff
- K&R declaration: same diff
- Hoisted vs unified `tail = n` factoring: hoisted is required to
  match the structural layout

## Permuter hint
5-instruction regalloc diff. Permuter should crack this in seconds.
The hoisted-tail factoring is the structural breakthrough — the
canonical C form (`if/else; tail = n;`) matched ~14 differs, the
hoisted form gets to 5.
