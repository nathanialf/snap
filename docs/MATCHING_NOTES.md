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
as a Y-axis rotation (E5C0) or Z-axis rotation (E460). Original stores
`1.0f` to the 4th-row diagonal slot (`dst[15]`) and re-reads it as
`lwc1 $f8, 0x3C($a0)` to use the same 1.0 for the 2nd-row (E5C0) /
3rd-row (E460) diagonal slot.

**2026-05-04 progress (still deferred but much closer):** the
volatile-reread trick triggers the `lwc1` reload at the right spot.
Source pattern that gets us there:

```c
dst[15] = 1.0f;
one = *(volatile f32 *) &dst[15];
... (other stores) ...
dst[10] = one;
```

With this the `swc1 $f6, 0x3C` / `lwc1 $f8, 0x3C` pair appears.
Remaining diff is a 4-byte float-regalloc swap: base picks
`$f4 = -sin / $f6 = 1.0` and `$f8` for the lwc1 reload; our build
picks `$f4 = 1.0 / $f6 = -sin` and `$f12` for the reload. Same
instructions, swapped FP registers — 4 byte differences total.

Tried for the regalloc swap: explicit `ns = -s;` decl reorder
ahead of `1.0`, intermediate `f32 one;` local, reorder of
unrelated stores. None flipped the FP allocation.

Earlier permuter from a worse seed plateaued at score 190 (vs base
285). With this near-miss seed (~16 bytes diff vs original 285)
the permuter should plateau much lower — re-run before further
manual variations. Tried earlier and failed: sequential stores,
reordered stores matching the asm flow, chained-assignment
(`dst[5] = dst[15] = 1.0f`).

### `func_80138DB0` — conditional getter (MATCHED)
Resolved 2026-05-04. Plain `if (D_80042D10 == 0) return 0; return
D_80042D18;` matches when compiled at **-O1** via the
`src/ultra_os_*.c` shim. The hoist of `lui $v0` into the delay slot
that breaks the match under the project default (-O2) goes away at
-O1.

This function is probably a libultra getter (or thin wrapper around
one) that was built with -O1 in the original tree, same as everything
under `os/` in `lib/ultralib/makefiles/ido.mk`. Original deferred
analysis below was correct about the hoist diff but missed the
flag-level workaround.

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

### `func_80138FA0` — osSetTime (MATCHED)
Resolved 2026-05-04. The function is libultra's `osSetTime`
(`lib/ultralib/src/os/settime.c`):
```c
void osSetTime(OSTime time) {
    __osCurrentTime = time;
}
```
The "spill-reload-without-frame" shape is just how IDO emits a u64
argument-to-global assignment under -O1: the u64 arg arrives in
$a0:$a1, gets spilled into the caller's positive spill slots, then
reloaded into $t6/$t7 to store to `__osCurrentTime`. Compiled via the
ultra_os_*.c shim (-O1). The pair `D_800994C0` (high) / `D_800994C4`
(low) is `__osCurrentTime` viewed as u64.

### `func_80137700` — osCreateMesgQueue (MATCHED)
Resolved 2026-05-04. The function is libultra's `osCreateMesgQueue`
(`lib/ultralib/src/os/createmesgqueue.c`). The "twin lui no-CSE" diff
that defeated earlier attempts comes from the cast pattern:
```c
mq->mtqueue   = (OSThread *) &__osThreadTail.next;
mq->fullqueue = (OSThread *) &__osThreadTail.next;
```
The pointer cast on each field assignment forces IDO to materialise
the same address twice — two `lui+addiu` sequences with no CSE — even
though the underlying address is identical. Compiled at -O1 via
`src/ultra_os_createmesgqueue.c` (CFLAGS_ULTRA_OS), with
`__osThreadTail` declared as `extern struct ... D_80042CB0` plus
`#define __osThreadTail D_80042CB0` to keep the source IP-clean.

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

### "IDO-picks-s0-over-stack-spill" cluster (MATCHED)
Resolved 2026-05-04. All four — `func_80135270`, `func_8013D820`,
`func_80138F50`, `func_8013B2D0` — match at -O1 via the
`src/ultra_os_*.c` shim. The s-register vs stack-spill divergence
that defeated all previous attempts at -O2 just goes away at -O1.

Recipe for `func_80135270/D820` (16-insn `lock → load → unlock →
return loaded value`):
```c
s32 func_xxxxxxxx(void) {
    register u32 saveMask = func_80038D70();
    s32 result = D_80042EBx[1];
    func_80038D90(saveMask);
    return result;
}
```

Recipe for `func_80138F50/13B2D0` (18-insn `lock → call body →
unlock → return body's result`). Critical: lock/unlock pair is
**no-arg** and the saveMask from lock is *not* threaded through
unlock; instead, `register s32 result` forces IDO to keep the body
return value in `$s0` across the unlock call:
```c
s32 func_xxxxxxxx(s32 arg0, s32 arg1, s32 arg2) {
    register s32 result;
    func_80035A30();
    result = func_8003B320(arg0, arg1, arg2);
    func_80035A74();
    return result;
}
```

The two cluster halves call different lock pairs:
`func_80038D70/90` (used by 80135270/D820) takes/returns a saveMask;
`func_80035A30/A74` (used by 80138F50/13B2D0) takes no args.

Generalises a useful pattern: when the diff is "IDO picks $s0,
ours uses stack spill," try `register` on the value that needs to
survive across a call, plus `-O1` via the ultra_os shim.

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

### `func_8010AD14` / `func_8010AD4C` — sound-API copy-and-cleanup pair (MATCHED)
Resolved 2026-05-04. Plain `void *` signatures with `(u8 *)+offset`
arithmetic match at -O2 — the same shape as the matched sibling
family `func_8010AC28/AC68`. The original deferred analysis claimed a
spill-order diff that turned out to be a misread; the prior attempts
must have used `s32 arg1` (which forces an integer copy through $v0
and breaks the body shape).

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

## func_8012AC50 (deferred — FP scheduling)

Computes `(s32)((f32)((f64)((f32)arg1 * (f32)arg0[17]) / D_800442B0 + 0.5))`. The
scalar form matches the asm structurally but IDO 7.1 schedules the integer→float
`mtc1`s and `cvt.s.w`s differently than the original:

- **Base order:** `mtc1 a1,f4 ; mtc1 t6,f8 ; cvt.s.w f6,f4 ; ldc1 f4,D ; ... ; cvt.s.w f10,f8 ; mul.s f16,f6,f10`
- **Mine:**     `mtc1 a1,f8 ; mtc1 t6,f4 ; cvt.s.w f10,f8 ; cvt.s.w f6,f4 ; ldc1 f4,D ; ... ; mul.s f16,f6,f10`

IDO chooses to do BOTH `cvt.s.w`s before `ldc1`, while the original interleaves
the second `cvt` AFTER the `ldc1` (so it can reuse `f4` for the constant). The
operand order to `mul.s` ends up swapped (`f6=arg0[17]_f, f10=arg1_f` vs base's
`f6=arg1_f, f10=arg0[17]_f`). The result is identical (mul commutes) but the
encoding diverges. Tried: intermediate `f32 a, b` locals (worse), single `f32 t`
intermediate (worse). No source-level construct found that forces IDO to defer
the second `cvt.s.w` past the `ldc1`. Permuter candidate.

## func_8012A640 (deferred — local placement)

f32-arg variant of the screen-message-send family. Sends type=4 message with
fval=arg1 to queue at arg0+0x14. Frame is -0x30, msg at sp+0x20. Tried:

- ANSI prototype + 16-byte struct: frame -0x28 (8 bytes too small)
- 24-byte struct (`s32 _wc[3]` padding): frame -0x30 ✓ but msg lands at sp+0x18 not sp+0x20
- Added unused `s32 _pad[2]`: discarded by IDO, no effect on layout
- K&R `f32 arg1`: promotes to double, completely diverges (ldc1 + cvt.d.s)

Need an additional 8-byte local that IDO won't strip. Maybe a buffer that's
written-to-then-read-from later in the call chain, or a struct with a non-trivial
init. Not obvious what the original source had. Permuter would likely find this
quickly given the small search space.

## func_80123074 (deferred — $a2/$a3 register-allocation choice)

Sound-channel midi-control sibling: byte store via D_800968C8[arg0] = arg1
gated by D_800968C4[arg0] != -1, then func_80032440(D_800968E4, byte_load).
Got down to 4-instruction diff: register choice swap between $a2 and $a3.

```
base: or $a3,$a0,$0  ; andi $a2,$a1,0xff  ; sb $a2  ; addu $t2,$t1,$a3
mine: or $a2,$a0,$0  ; andi $a3,$a1,0xff  ; sb $a3  ; addu $t2,$t1,$a2
```

`u8 arg1` K&R produces the spill + andi correctly (good progress vs the
no-spill no-andi initial pass), but IDO assigns $a2 to whichever value is
referenced FIRST in the body — and `D_800968C8[arg0] = arg1` reads arg0
(for index) before arg1. Tried swapping eval order via `u8 v = (u8)arg1;
D_800968C8[arg0] = v;` (lost the spill), `register u8`, K&R reorder. Permuter
candidate.

## func_80135560 / __osSiRawStartDma (deferred — eager arg0 spill)

Direct port of lib/ultralib/src/io/sirawdma.c (VERSION_J variant: SI_STATUS_REG
& (DMA_BUSY|RD_BUSY) in place of __osSiDeviceBusy). Snap's symbols:
__osDpDeviceBusy=80037860, osWritebackDCache=80034DB0, osVirtualToPhysical=80034D30,
osInvalDCache=800377B0.

Diff is at the function entry: base spills only $a1 (dramAddr) and copies
arg0 to $a2 (`or $a2, $a0, $0`); mine spills both $a0 and $a1 eagerly. -O1
ultra_os_*.c shim, ANSI prototype, with or without `register s32 direction`
or `register u32 stat`. Adding the `stat` local pushed it to frame -0x28
plus $s0 spill (totally diverged).

Likely needs an alt source-side trick — `register void *dramAddr` to push
dramAddr (not direction) to a callee-save register? Skipping for now; the
match_check diff was clear enough that a permuter run should crack it.

## func_8011E460 / func_8011E5C0 / func_8011E2B0 (deferred — FP register choice)

Z/Y/multi-axis rotation matrix builders. Pattern: call sinf+cosf (func_80032A20,
func_80038DE0), build 4x4 matrix, return. Got down to 4-byte register-choice
diff (f4 vs f6 swap):

```
base:  mtc1 at,$f6 ; neg.s $f4,$f14 ; swc1 $f6,0x3C(a0) ; swc1 $f4,0x10(a0)
mine:  mtc1 at,$f4 ; neg.s $f6,$f14 ; swc1 $f4,0x3C(a0) ; swc1 $f6,0x10(a0)
```

Required `m[10] = *(volatile f32 *)&m[15]` to force the load-after-store dance
(otherwise IDO reuses the const register for both stores). With that, only the
register choice differs. Tried explicit `f32 ns = -s` local — no change.
Permuter candidate. Likely all three rotation builders crack with the same
trick once one does.

## func_8010AD14 (deferred — register choice)

3-arg-style sibling of func_8010ACAC. After func_800087AC call: reload
arg1 + read its 0x28 field, copy to arg0+0x28, then func_80008650(arg0).
Got down to 3-instruction register-choice diff: base reloads arg1 into
$a1 and uses $t6 for the 0x28-load; mine reloads to $t6 and uses $t7.

```
base:  lw $a1,0x1c($sp) ; lw $t6,0x28($a1) ; sw $t6,0x28($a0)
mine:  lw $t6,0x1c($sp) ; lw $t7,0x28($t6) ; sw $t7,0x28($a0)
```

Tried: ANSI prototype, K&R, `register void *arg1`. None coerce IDO
into the $a1-reload pattern. Permuter candidate.
