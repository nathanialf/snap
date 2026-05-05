# func_8011FD74 — bump allocator (regalloc differences)

5-arg K&R-style bump allocator. Allocates count*elem_size rounded up
to 16-byte alignment from pool[1]. Returns the old cursor or 0.

## Diff (13/21)

Shape correct but many regalloc differences. K&R declaration spills
all 5 args but compiler chooses different t-regs for the math.
