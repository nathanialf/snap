# func_8013125C — linked-list remove (sibling of func_801311F8)

**ROM offset:** `0x31E5C` **size:** `0x70` (28 instructions)
**VRAM:** `0x8013125C`

## Behaviour
Walk arg0->head looking for the node at `arg1 - 4`. If found, unlink
it: update head/prev->next, possibly update tail, push to free-list.
**Returns `prev`** (the node before target, or 0/last if not found),
not the unlinked target.

## What's matching
- `target = arg1 - 4` precomputation
- bnel-likely loop with `prev = cur` in delay slot
- beql-vs-prev==0 dispatch (after if-condition inversion to
  `if (prev != 0)`)

## Remaining diff (~9 differing instructions)
Register allocation: base uses `$v1` for `cur`, `$v0` for `prev`,
`$a2` for `target`. Built ends up with `$a2` for `cur`, `$v1` for
`prev`. Reordering local declarations didn't fix it.

Plus a scheduling difference around the unlink-store sequence: base
has `or $v0, $v1, $zero` placed AFTER `jr $ra; sw $v1, 0x70` (dead
code from natural control flow) — might be IDO emitting the return
value move into a slot the scheduler couldn't consume. Built has
the move BEFORE the sw instead.

## Variants tried
- Declared `cur` first vs `prev` first vs `target` first.
- `if (prev == 0) ... else ...` vs the `!= 0` form (the != 0 form
  matched the function-sibling func_801311F8 — but here it changes
  dispatch direction inside the if-found branch).
- Unrolled the `do { ... } while` into a for loop: ⌐no help.

## Permuter hint
Sibling of `func_801311F8` (matched). Source body is logically
correct — the permuter needs to find the regalloc combination that
keeps `cur` in `$v1` and `prev` in `$v0`. Try `register Node *cur`
and `register Node *prev` with various orderings.
