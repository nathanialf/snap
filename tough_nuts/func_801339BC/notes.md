# func_801339BC — sorted-list (descending) insert

Walks `*arg0` linked list of nodes with key at offset 4. Inserts arg1
before the first node whose key is < arg1->_4. Stores the container
pointer arg0 into arg1[2] (back-link).

## What's matching (~17/21 instructions)
- Logic: walk while cur->_4 >= key, insert before
- Final 5 instructions (sw cur, sw newnode, jr ra, sw arg0)

## Remaining diff
- Mine uses **bnezl/beqzl** (branch-likely) with sw v1 / move v0 in
  delay slots; target uses **bnez/beqz** with explicit nop delay slots
- Different register allocation: target uses t8/t9/t7/t6, mine uses
  v1/v0/a2/t6

## Variants tried
- `while(cond)` → branch-likely emitted
- `if(cond) do {...} while(cond)` (recipe from saved memory) → same diff
- Both produce branch-likely

## Permuter hint
Same regalloc + branch-likely-vs-regular plateau. Permuter target.
This file (asm/33EA0.s) has 5 functions sharing a TU; would need to
match all of them OR use INCLUDE_ASM for the unmatched ones to carve
just this one.
