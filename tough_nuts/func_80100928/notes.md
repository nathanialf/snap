# func_80100928 — short-circuit loop with all-three globals hoisted

`while (D_8004873C != 0 || D_80048754 != 0 || D_8004874C != 0)
func_80000920();` — but base hoists ALL three globals into saved
registers (s0, s1, s2) BEFORE the loop and uses lw-from-saved-reg
inside, even though only one short-circuit branch needs each.

## Diff

Built lazily computes addresses (lui+addiu) for the second and
third globals only when reached, costing extra LUIs inside the
loop body.

## Variants tried

- Plain global access: `while (A != 0 || B != 0 || C != 0)` — lazy
- Pointer locals: `s32 *p0=&A; ... while (*p0 || *p1 || *p2)` — lazy

## Permuter hint

Try a `do { ... } while`-shaped loop, or "prime" all three globals
into local s32 reads before the loop and pass them through. May
need a register hint or a single combined boolean computed first.
