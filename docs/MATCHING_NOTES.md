# Matching notes & deferred targets

Workflow tips that have already paid for themselves; details on functions
that resisted a quick match and what to investigate next.

## Workflow

- `tools/find_leaf_candidates.py --max-insns N [--no-branch]` ranks
  asm/*.s files by ease — pure leaves (no `jal`), short, optionally no
  branches.
- After editing C, `tools/match_check.py 0xADDR:0xSIZE` reads the linked
  ROM and the baserom and prints word-by-word side-by-side diffs without
  parsing make output.
- Splat is slow (~60s). Batch many yaml flips into one `make split`.
- When iterating on a single C source, you don't need to re-split — just
  edit `src/foo.c` and rerun `make`. Splat only needs to run when yaml
  subsegment ranges change.
- Hard bound on investigation per function: if it doesn't match in 2-3
  attempts, revert and add to "tough nuts" below. Don't sink hours into
  one byte-level codegen difference; come back when patterns from other
  matches give us more leverage.

## Confirmed match patterns

- **Plain getter** (`extern T D_xxx; return D_xxx;`) matches when the
  return register and address-load register are the same (`$v0` for
  s32). See `func_8013D0B0`, `func_801385B0`, `getters.c`.
- **Hardware register r/w** uses `*(volatile T *)0xADDR` (NOT `extern T
  D_xxx`). Plain extern routes the address-load through GNU `as`'s
  `$at` macro temp, but the original ROM was assembled with IDO's
  `as0/as1` which picks `$t6` etc. The volatile-pointer pattern
  bypasses the macro and emits `lui $t6` directly. See
  `func_8013CD80/90`.
- **Multi-store struct init** with `arg0[i] = ...` and
  `*(s16 *)((u8 *)arg0 + N) = 0` for half-word fields matches reliably.
  See `func_8012DE50`, `func_80126140`.
- **Sign-extend store** (`*(s32 *)... = (s16)arg`) matches even with
  the IDO arg-spill quirk on the input register. See `func_8012A500`,
  `func_80131A90`.
- **Bit-clear on packed field** (`*(u16 *)... &= ~arg`) matches; IDO
  emits `not $t, $a; and $t, $r, $t` cleanly. See `func_801287F0`.

## Tough nuts (deferred)

### `func_80138DB0` — conditional getter
The original returns `D_80042D10 ? D_80042D18 : 0` but does **not**
hoist the second `lui` past the branch:
```
lw  $t6, D_80042D10
bnez $t6, .L
nop                    <-- delay slot is plain nop
jr  $ra
or  $v0, 0, 0          <-- delay slot of jr (return 0)
.L:
lui $v0, %hi(D_80042D18)   <-- NOT hoisted up
lw  $v0, %lo(D_80042D18)($v0)
jr  $ra
nop
```
Both `if (!D_80042D10) return 0; return D_80042D18;` and the ternary
form produce the IDO `-O2` scheduler hoisting the `lui $v0` into the
nop delay slot above the branch. Marking `D_80042D18` `volatile` does
not help (volatile semantics apply only to the `lw`, not the
independent `lui`). Likely fix: a different IDO version or scheduler
flag, or a pragma we haven't found yet.

### `func_80138FA0` — two-global setter
Original spills both args to the caller's arg-spill area on `$sp` and
reloads through different temps before storing — without ever
allocating a stack frame:
```
sw $a0, 0(sp)          <-- spill into caller's spill area
lw $t6, 0(sp)          <-- reload to t6
sw $a1, 4(sp)          <-- same for arg1
...
sw $t6, %lo(D_800994C0)($at)
sw $t7, %lo(D_800994C4)($at)
```
Tried: `volatile` locals (allocates own frame, wrong shape),
`s32 *p = &arg0; p[0]/p[1]` (IDO collapses), `struct Pair p`
(IDO spills extra dead stores). The original looks like K&R-style or a
varargs decl. Worth revisiting once we identify the source style.

### `func_80137700` — twin function-pointer init
Original loads `&D_80042CB0` into TWO different temp regs ($t6 and $t7)
and stores both to fields 0 and 4 of a struct:
```
lui $t6, %hi(D_80042CB0)
lui $t7, %hi(D_80042CB0)
addiu $t6, ...; addiu $t7, ...
sw $t6, 0($a0)
sw $t7, 4($a0)
```
IDO normally CSEs the two loads of the same address. The original
either had two distinct symbols at the same address, or used a pattern
that defeated CSE (e.g. function-pointer cast that the optimizer
treats as opaque). Likely fix: declare `D_80042CB0` once as a function
and one cast as `(s32)&D_80042CB0` and one through a different alias.

### `func_80135270` / `func_8013D820` — int-mask-protected ptr deref
Identical 16-insn shape: `__osDisableInt`-style call → load `*(D_80042EBx + 4)`
→ `__osRestoreInt`-style call → return loaded value. The original IDO
allocates `$s0` for the saveMask (`or $s0,$v0; or $a0,$s0`) even though
the lifetime never crosses a function call. Our build skips the s0
allocation entirely and emits `or $a0,$v0` with no s-reg save. Tried:
plain code, `register s32 saved`, `register u32 saveMask`. Permuter ran
~51k iterations at speed 100 without scoring below 420 (vs base 465).
The fact that other matched callers of `func_80038D70`/`90` (e.g.
`func_80137B10`) spill `$v0` to stack instead of allocating `$s0`
suggests something specific in the source-level structure of these two
funcs forces s-reg allocation — possibly an extra inline-helper call or
an unusual `register` placement. Defer until we learn the libultra-style
idiom variant the original used.

### `func_80137860` / `func_80137890` / `func_8013CD50` — HW-reg bit tests
Identical shape: read a hardware register, mask, return 1/0. Original
allocates an empty 8-byte stack frame (no spills) and uses `$a0` as
the register-value temp:
```
lui  $t6, ...
lw   $a0, ...           <-- into $a0 (a no-arg function!)
addiu $sp, $sp, -8      <-- empty frame
andi $t7, $a0, MASK
beqz $t7, .L1
nop
b    .L2
addiu $v0, 0, 1         <-- delay slot, true case
.L1:
or   $v0, 0, 0          <-- false case
.L2:
jr   $ra
addiu $sp, $sp, 8       <-- frame teardown in delay
```
Tried `if (...) return 1; return 0;`, ternary form, dummy local. None
produce both the empty stack frame AND the join-merge layout. The
empty frame strongly suggests the original source used an inline
helper from a header that left `$sp` adjustments behind, or compiled
with a `livereg` directive we haven't replicated.

## Ideas worth trying next

- Decompile non-leaf functions whose callees can stay as asm — the
  linker will resolve `jal func_X` against the still-asm version.
  `func_80128970` and friends call `func_80029E80` (likely
  `osSendMesg`), and the call patterns match standard libultra usage.
- Carve obvious data-only "functions" (`func_801DDxxxx`, etc) into
  `data` subsegments — they're disassembled as nop-then-`.word`
  blobs because spimdisasm couldn't tell code from data.
- Look up which IDO 7.1 sub-version `ido-static-recomp` ships and
  whether the original ROM used a different one. The hoist behavior
  on `func_80138DB0` and the empty-frame pattern on the HW-bit-test
  trio both feel like compiler-version differences.
