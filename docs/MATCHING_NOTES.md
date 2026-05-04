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
- **Mtx-build wrapper family** (mixed s32 + f32 args, build a matrix
  on local stack and apply it). Signature shape:
  `void f(s32 obj, f32 b, f32 c, s32 mode, f32... rest)`. Body shape:
  `s32 buf[16]; build(buf, b, c, mode^0, rest...); apply(buf, obj);`.
  Match recipe: ANSI prototype with the actual mixed types, but pass
  the lone s32 mid-arg as `mode ^ 0` so IDO emits the dead-spill of
  `$a3` to its home slot (K&R would force the spill but breaks the
  f32 args via cvt.s.d). Confirmed on:
  - `func_8011E25C` / `func_8011E40C` (6 args, helpers
    `func_8001E20C` / `func_8001E3BC` + `func_8001B320`)
  - `func_8011B7FC` / `func_8011E08C` (10 args, helpers
    `func_8001B570` / `func_8001E028` + `func_8001B0F0` /
    `func_8001B320`)
  - `func_8011BB48` / `func_8011D1F0` (11 args)

## Tough nuts (deferred)

### `func_8011E460` / `func_8011E5C0` — single-axis rotation matrices
36-insn pair, both pure leaves: take `(f32 *dst, f32 angle)`, call
sin (`func_80032A20`) and cos (`func_80038DE0`), then fill `dst[0..15]`
as a Y-axis rotation (E5C0) or Z-axis rotation (E460). Plain
sequential-store source matches every byte except for one IDO -O2 CSE
trick: the original stores `1.0f` to the 4th-row diagonal slot
(`dst[15]`) and then *re-reads* it as `lwc1 $f8, 0x3C($a0)` to use the
same 1.0 for the 2nd-row (E5C0) / 3rd-row (E460) diagonal slot, instead
of materialising the constant twice. Tried: sequential stores,
reordered stores matching the asm flow, chained-assignment
(`dst[5] = dst[15] = 1.0f`). All produced the stores but none of them
emitted the lwc1 reload. Permuter ran ~11500 iters @ speed 100,
plateaued at score 190 (vs base 285) — best candidates inserted dummy
`if (1)` blocks and `dst[10] = (dst[11] = c)` chains that mirror the
shape but not the byte sequence. Defer until a source-side trigger for
"materialise constant once, reload from store-target for second use" is
found.

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

### `func_8010EB28` / `func_8010EB98` — switch with absolute jumptable
28-insn sibling pair, both pure switch-on-arg1 with 10 cases each
returning a different `f32` field of `arg0`. The body bytes match
trivially with a plain `switch` statement — the only diff is the two
instructions that load the jumptable address:

```
base:  lui $at, %hi(jtbl_800438A8_main)   ; lw $t6, %lo(...)($at)
built: lui $at, 0x8110                    ; lw $t6, -0xC00($at)
```

The original ROM's jumptable lives at VRAM `0x800438A8` (and
`0x800438D0`), which is *below* the main segment's VRAM start
(`0x80100400`) — outside any segment we currently carve. splat has
recorded these as absolute symbols (`jtbl_800438A8_main` etc. in
`config/undefined_syms_auto.us.txt`), but our compiler emits its own
jumptable into our `.rodata` at a high address. ROM bytes therefore
diverge at the lui/lo immediates.

Until we either (a) carve the rodata segment that contains these
jumptables and reference them as `extern` (computed-goto, not standard
C — IDO support TBD), or (b) figure out how the original linker
positioned per-function rodata at low VRAM, switch statements with
external jumptables are unmatched. Defer.

### `func_80123074` — sound-channel byte-set, $a2/$a3 swap
26-insn leaf in the sound-channel module. Stores `arg1` as a byte to
`D_800968C8[arg0]` then calls `func_80032440(D_800968E4, D_800968C4[arg0])`.
Five source variants tried (ANSI int, ANSI int+`&arg1` home, ANSI
`(u8)arg1` cast, `arg1 & 0xFF`, K&R `unsigned char arg1`). The K&R
`unsigned char` form is closest — exactly four byte diffs remain, all
the same regalloc swap:

  - base: `or $a3,$a0,0` (preserve arg0 to $a3) +
    `andi $a2,$a1,0xFF` (mask into $a2) +
    `sb $a2` + `addu …,$a3`
  - built: `or $a2` + `andi $a3` + `sb $a3` + `addu …,$a2`

Pure register-allocation flip — IDO picks $a2-low for what base
picks $a3-low and vice versa. No source-side trigger found; this is
permuter territory (decomp-permuter likely cracks it in <1k iters
since the diff is 4 instructions of swapped allocation). The
neighboring siblings `func_80122D54 / E20 / EEC / FD0` matched on
the first/second attempt with K&R int + `(u32)arg1 < 0x80U` (sound
critical-section) — the regalloc twist is specific to 80123074's
shape (no critical section, no clamp, just byte-write + call).

### "IDO-picks-s0-over-stack-spill" cluster — `func_80135270` / `func_8013D820` / `func_80138F50` / `func_8013B2D0`
Family of small libultra-style "lock + body + unlock" wrappers where
the value returned from the middle call has to survive across the
unlock call. The original IDO allocates `$s0` to hold that value
(adding `sw $s0` / `lw $s0` to prologue/epilogue and an extra `or
$s0,$v0; or $v0,$s0`), but our IDO 7.1 decides spill-`$v0`-to-local
is cheaper and skips the s-reg allocation entirely. Net effect: a
~4-insn frame-size + epilogue diff that no source-side tweak we've
tried (plain code, `register`, K&R live-spill, ordering) reproduces.

`func_80135270/D820` (16-insn): `func_80038D70()` → load
`*(D_80042EBx + 4)` → `func_80038D90(saved)` → return loaded value.
Permuter ran ~51k iters @ speed 100, never scored below 420 (base 465).

`func_80138F50/13B2D0` (18-insn): `func_80035A30()` → spill 3 args →
`result = func_8003B320(a,b,c)` (or `func_8003BB20`) → `func_80035A74()`
→ return result. Same s0-allocation gap.

Other matched callers of these helper pairs (e.g. `func_80137B10`,
`func_80124F54`) take the spill-to-stack path *and* match — so the
behavior is definitely consistent between our IDO and the original
IDO; the original's choice to use s0 in *these* specific functions
must come from a source-level construct we haven't identified yet.
Deferred as a single class — cracking one will unblock the others.

### `func_80135270` / `func_8013D820` — int-mask-protected ptr deref
See above (folded into the shared "IDO-picks-s0-over-stack-spill"
cluster).

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

### `func_8010AD14` / `func_8010AD4C` — sound-API copy-and-cleanup pair
Adapter shape: `func_800087AC(arg0, arg1)`, then
`*(s32 *)((u8 *)arg0 + 0x28) = *(s32 *)(arg1 + 0x28)`,
then `func_80008650(arg0[, *(s32 *)(arg1 + 0x8)])`. AD14 is 14 insns
(1-arg cleanup), AD4C is 15 insns (2-arg cleanup). Body bytes match
when arg1 is `s32` and explicitly passed to `func_80008650`. The
remaining diff is purely prologue spill order: base spills `sw $a1`
BEFORE the `jal func_800087AC` call and `sw $a0` IN the delay slot,
but our IDO 7.1 emits the opposite order (`$a0` before, `$a1` in
delay). Plus, AD4C reloads arg1 into `$v0` (base) vs `$a2` (built) —
same family of regalloc divergence as the IDO-picks-s0 cluster, but a
different choice point.

Sibling matched pair `func_8010AC28/AC68` in the same neighborhood
has `a0`-then-`a1` spill (matches our IDO), and matched
`func_8010ACAC/ACE0` has `a1`-then-`a0` spill (like AD14/AD4C base).
The trigger isn't arg1 type alone (both ACAC and AD use `s32 arg1`)
nor arity of the second call (ACAC and AD14 both call 2-arg). The
distinguishing factor between the AC and AD families is unclear.

Permuter ran 24151 iters @ default speed, lowest score 25 (base 225).
Best candidate inserted `s32 new_var = arg1; arg1++; arg1--;` to
shake regalloc — got close but not matching.

Deferred. Revisit when we crack the IDO-picks-s0 cluster — both
likely come from the same regalloc heuristic difference.

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
