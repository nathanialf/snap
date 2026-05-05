# func_8011FD74 — arena allocator (16-byte aligned bump)

**ROM offset:** `0x20974` **size:** `0x54` (21 instructions)
**VRAM:** `0x8011FD74`

## Behaviour
5-arg K&R function. The first two args are spilled but unused. The
real args are arena ($a2), count ($a3), size (sp+0x10).

```c
void *func_8011FD74(arg0, arg1, arena, count, size)
s32 arg0, arg1;          /* unused, K&R spilled to home */
Arena *arena;
s32 count, size;
{
    void *result = 0;
    s32 sz = (count * size + 0xF) & ~0xF;  /* round up to 16 */
    if (arena->start + arena->cap >= arena->cur + sz) {
        result = arena->cur;
        arena->cur += sz;
    }
    return result;
}
```

`Arena = { u8 *start; u8 *cur; s32 cap; }` (12 bytes).

## What's matching
- K&R spill of $a0/$a1 to home positions
- multu count*size with mflo
- (+0xF) & ~0xF align-up
- sltu unsigned compare for overflow check
- Single basic-block branch with `arena->cur += sz` on success path

## Remaining diff (~9 instructions, all regalloc)
Base loads arena->cap/start/cur into `$t8`, `$t7`, `$a0` (reusing
spilled $a0). Built loads them into `$t1`, `$t0`, `$v0`. Same logic,
different register assignment. The use of `$a0` post-spill is the
distinctive thing about base — built didn't pick `$a0`.

## Variants tried
- Saved version uses post-spill `arg0` is still in source signature
  (K&R unused-arg form).

## Permuter hint
Pure regalloc. Permuter should crack quickly. Look at how IDO chose
to reuse `$a0` post-K&R-spill in base — there may be a source-side
trigger (e.g., `arg0` referenced once before use, or specific
declaration order).
