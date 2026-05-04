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
  - `func_8011DFCC` (7 args, helper `func_8001DF78` + `func_8001B320`)
  - `func_8011BFAC` (11 args, helper `func_8001BBC4` + `func_8001B0F0`,
    mode-as-arg1 variant — `arg1` passes straight through without a
    spill so no `^ 0` trick is needed)

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
picks $a3-low and vice versa. No source-side trigger found.

Permuter attempted from ANSI seed (`s32 arg0, s32 arg1` with
`s32 *home = &arg1` to mimic the K&R spill): 28k iters @ speed 100,
4 workers, plateau score 140. Lowest-score candidate inserts
`int new_var; new_var = arg1; D_800968C8[arg0] = new_var;` — extra
local store but no regalloc change. The K&R 4-byte near-miss
(starting score ~40 equivalent) is **not reachable** from any ANSI
seed via the permuter's syntactic mutations: the K&R-char-spill
behavior is a parser-level shape that ANSI source doesn't emit and
the permuter can't synthesise. Permuting a K&R seed isn't possible
either — `permute_run.sh`'s pycparser frontend rejects K&R syntax.

Next angles to try (when revisiting):
- A source-level trigger on the K&R form that flips regalloc:
  reorder param decls (`unsigned char arg1, s32 arg0`?), make `arg0`
  preservation explicit (`s32 saved = arg0`), or rewrite the call
  site to swap which arg is held across.
- A side-by-side build of one of the matched callers (e.g.
  `func_80124F54`) that takes the spill-to-stack path and matches —
  identify what differs at the source level.

The neighboring siblings `func_80122D54 / E20 / EEC / FD0` matched on
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

### `func_80137860` / `func_80137890` / `func_8013CD50` — HW-reg bit tests (MATCHED)
Resolved 2026-05-04. These are libultra `__osSpDeviceBusy`,
`__osSiDeviceBusy`, and `__osDpDeviceBusy` (`lib/ultralib/src/io/sp.c`,
`si.c`, `dp.c`). The empty 8-byte frame + `$a0` load shape comes from
**-O1 + `register u32 stat`**, not from any header-inlined helper.

Recipe:
```c
s32 func_8013xxxx(void) {
    register u32 stat = *(volatile u32 *) 0xA404____;
    if (stat & MASK) {
        return 1;
    } else {
        return 0;
    }
}
```
Naming the source file `src/ultra_os_<name>.c` routes it through
`CFLAGS_ULTRA_OS` (-O1) which is what triggers the `addiu $sp,-8 / +8`
frame and the `lui/lw $a0` load order. The project's default `-O2` for
this shape produces a frameless `lui/lw $v0` form that diverges by
~10 bytes.

Confirmed on:
- `func_80137860` = `__osSpDeviceBusy` (SP_STATUS_REG, mask
  `SP_STATUS_DMA_BUSY|SP_STATUS_DMA_FULL|SP_STATUS_IO_FULL` = 0x1C).
- `func_80137890` = `__osSiDeviceBusy` (SI_STATUS_REG, mask
  `SI_STATUS_DMA_BUSY|SI_STATUS_RD_BUSY` = 0x3).
- `func_8013CD50` = `__osDpDeviceBusy` (DPC_STATUS_REG, mask
  `DPC_STATUS_DMA_BUSY` = 0x100).

### `func_80119E78` — RNG-byte → call → mul.s scheduling
14-insn function. Body: `func_8003D478(0, (u8)func_80037EE0()) *
(1.0f / 256.0f)` — sample a u8 from `func_80037EE0` (a u64 RNG; same
idiom matched on `func_80119E54`), pass to `func_8003D478` (returns
f32), scale by 1/256. Five bytes of pure scheduling diff at the
epilogue: base puts `lui $at; mtc1 $at,$f4; lw $ra; addiu $sp;
mul.s $f0,$f0,$f4; jr $ra; nop` (mul.s before jr, nop in delay slot)
while our IDO 7.1 emits `lw $ra; lui $at; mtc1; addiu $sp; jr $ra;
mul.s` (mul.s in the delay slot of jr).

Source variants tried: one-liner, intermediate `f32 v = ...` then
return v * scale, cast-then-call. None move the mul.s out of the
delay slot — IDO 7.1's scheduler aggressively fills jr's delay slot
with the mul.s and we found no source-side knob to suppress that.

Permuter ran ~22k iters @ speed 100, base score 120, plateau 105.
Best candidate inserted `float new_var; float *new_var2 = &new_var;
return *new_var2;` — forces a stack store/reload but doesn't
change the mul.s scheduling. The 5-byte diff is below the permuter's
mutation vocabulary from any clean ANSI seed.

Suspect a compiler-version difference (IDO 7.1 sub-version or a
`-O` variant) is the real driver, similar to the empty-stack-frame
HW-bit-test trio. Defer; revisit when we identify the IDO patch
level the original ROM used.

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

### `func_80133A04` — list-pop one-liner with $v0/$t9 regalloc
4-insn function, ROM-bytes pattern:
```
lw   $v0, 0x0($a0)
lw   $t9, 0x0($v0)
jr   $ra
sw   $t9, 0x0($a0)   ; (delay slot)
```
Body shape `*arg0 = (*arg0)->next; return old;`. With explicit return-
value plumbing (signature `Node *func(Node **arg0)` returning the old
head) we get the FIRST temp into `$v0` (matches), but the second temp
lands in `$t6` instead of the expected `$t9` — IDO's allocator picks
the next sequential temp register, not `$t9`.

Tried: nested-deref one-liner, explicit local intermediate, struct-
typed `Node` with `->next`, `void*`/`u8*`/`u32*` casts. All produce
either `$t6/$t7` (no return) or `$v0/$t6` (with return). Permuter ran
~250 iters @ 5 min, plateau score 10 (base 10), no improvement —
mutations like `if (1){}`, dummy temps, chained assignments all stayed
at the same diff.

The `$t9`-as-second-temp choice may be tied to: (a) function-pointer-
register pun (IDO recognises that the loaded value will be jumped
through next), (b) a specific cast/typedef that signals to IDO's
allocator. Not yet found. Permuter best candidate inserted `new_var2
= arg0; ... return *new_var2;` — same plateau.

Deferred. Revisit if a future match cracks the `$t9`-second-temp
pattern, or if we figure out the IDO heuristic that picks high-temp
registers for follow-the-pointer chains.

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
