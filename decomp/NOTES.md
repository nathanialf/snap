# Decomp toolchain gotchas

Living checklist of every IDO 7.1 / GNU `as` / splat / linker quirk that
has bitten matching work in this repo. Read this before starting a new
match — most "near-miss" diffs are an instance of one of these patterns.

For deeper dives on individual deferred functions, see
[`docs/MATCHING_NOTES.md`](../docs/MATCHING_NOTES.md). For per-pattern
playbooks (volatile-pointer trick, K&R sub-variants, bnel idiom, etc.),
the saved-memory feedback files in
`~/.claude/projects/-primary-dev-snap/memory/` index them all.

---

## ultralib vendoring (IP exception)

Vendoring `decompals/ultralib` (its `.c` source and `include/PR/`
headers, including the SGI/Nintendo copyright text) is permitted in this
repo, matching community norm across N64 decomp projects (OoT, MM,
MK64, Banjo-Kazooie, Yoshi's Story, etc.). This exception applies
**only** to ultralib; all other Nintendo-derived material (game source,
leaked development docs, asset extractions) remains forbidden.
ultralib lives at `lib/ultralib/` and is gitignored — clone it locally
via `git clone --depth 1 https://github.com/decompals/ultralib lib/ultralib`.

We currently target SDK release **2.0J** (Snap shipped 1999, between
Yoshi's Story which uses 2.0I and OoT which uses 2.0L; 2.0J is the
era-appropriate pick and is supported by ultralib).

## Compiler choice & flags

- **IDO 7.1 is the matching compiler.** GCC won't match. `cc1` / clang
  won't match. Installed externally via `ido-static-recomp`; default
  `~/.local/share/ido-recomp/build/7.1/out/`. Override via `IDO_DIR=`.
- **Use exactly these CFLAGS:**
  `-c -G 0 -non_shared -Xfullwarn -fullwarn -O2 -mips2 -woff
  649,838,712,807 -signed -nostdinc -Iinclude`.
- **Never add `-g3`.** It suppresses delay-slot scheduling — even a
  trivial leaf will diverge in the jr/delay byte if `-g3` is on.
- **`-mips2` not `-mips3`.** Means 64-bit ops are lowered to runtime
  helpers (`__ull_rem`, `__ll_div`, …). Functions in the ROM that
  inline `ddivu`/`ddiv` were built with different flags (likely
  `-mips3`); we cannot match those with the project's CFLAGS without
  changing them. Defer such functions or build them in their own .c
  file with a per-target flag override.

## Assembler / linker wrinkles (already handled — do not re-debug)

- **`bltzal $ra` rejected by GNU `as`.** Two real instructions in the
  ROM use this pattern. `tools/postprocess_asm.py` rewrites them as raw
  `.word` after every `make split`. Don't disable it.
- **Float-register-should-be-even warnings.** Many real instructions
  use odd-numbered FP registers in the 32-bit ABI. Warnings only,
  ignore.
- **Undefined references to `func_<high>` / `D_<high>`.** spimdisasm
  sometimes interprets a literal data word as a `lui+addiu` code
  pointer, producing references to addresses far outside the code
  range. `tools/gen_symbols_from_asm.py` regenerates
  `config/undefined_funcs_extra.us.txt` after every split to declare
  these as absolute symbols so the link succeeds. The symbols are
  never executed; they exist only to pass the linker.
- **LD warning about RWX LOAD segment.** Cosmetic, harmless on N64.
- **`NULL` is not defined** in `common.h` for IDO. Use bare `0` for
  pointer comparisons. `cfe: 'NULL' undefined` is the error to
  recognise.

## Splat / yaml subsegment pitfalls

- When carving a C file out of an asm subsegment, **explicitly add asm
  subsegments before and after** the new C file or splat will leave
  the surrounding bytes orphaned and the SHA-1 will diverge. Pattern:
  `[asm] [c, func_X] [asm] [c, func_Y]`, never `[c, func_X] [c,
  func_Y]` unless the C file actually defines every function in the
  range.
- After yaml edits, **run `make split` once** before `make`. Splat
  rewrites the linker script and asm files; `make` alone won't pick
  up subsegment changes.
- If a build hits a 14M-byte SHA diff after a yaml edit, the build
  artifacts are stale. **`rm -rf build/ && make split && make`** fixes
  it. Splat's incremental cache occasionally drops bytes after
  subsegment boundary changes.
- **`match_check.py` takes ROM offsets, not VRAM.** Subtract
  `0x80100400 - 0x1000 = 0x800FF400` from the VRAM, or read the asm
  comment `/* ROM_OFFSET VRAM …` for the right side. A VRAM in
  `match_check.py` is silently past EOF and returns a false MATCH.

## IDO codegen patterns to recognise

These are the source-side tricks IDO 7.1 was happy to emit but modern
C makes hard to write. If your near-miss looks like one of these,
switch to the listed pattern.

### Hardware register / volatile load
Original asm has `lui $t6, …; lw $vN, …($t6)` with `$t6` as the
address-load temp (instead of `$at` / `$v0`). Source is
`*(volatile T *)0xADDR`, **not** `extern T D_xxx;`. The plain extern
goes through GNU `as`'s macro temp; the volatile pointer bypasses it
and emits `lui $t6` directly. See `func_8013CD80/90`,
`feedback_snap_matching_volatile_ptr.md`.

### K&R declarations
When the asm spills incoming arg registers to the **caller's** spill
area (positive offsets above the new `$sp` relative to the frame
size) and the spills are dead or live across calls, the source uses
K&R-style decls:

```c
void f(thr, arg1, arg2)
s32 *thr;
char arg1;
int arg2;
{ ... }
```

ANSI prototypes skip the eager spill of all args because IDO can prove
they live only in registers. K&R always spills to the canonical home
slot.

Sub-variants — count and shape of spills classifies them:

- **plain dead-spill** — K&R `int arg`, never reloaded. Bare spill at
  the prologue. Most common.
- **live-spill** — K&R `int arg`, spilled at prologue and reloaded
  after a call (`sw $aN; jal …; lw $tN, same_slot(sp)`).
  ~169 unmatched functions in this bucket — unblocking it is the
  single biggest payoff lever.
- **byte-narrow-via-memory** — K&R `char arg`, read with
  `lbu $tN, sp+offset+3` from the spill slot. Source pattern:
  `((unsigned char *)&arg)[3]`, **not** `(s8)arg`. Big-endian, so byte
  3 is the low byte. The `&arg` forces it into memory at its spill
  slot. See `feedback_snap_kr_byte_narrow.md`.
- **halfword-narrow-via-memory** — same idea but `u16 arg` →
  `lhu sp+offset+2`. K&R decl alone is enough; no `((u16 *)&arg)[1]`
  trick needed.
- **struct-by-value** — direct halfword stores to globals + dead
  `sw $a0/$a1` spills. Source: `Vec2i v` / `Vec3 v`, not K&R int.
  See `feedback_snap_struct_by_value.md`.
- **K&R char + andi vs lbu (ambiguous)** — IDO has two valid forms
  for a K&R `u8 arg` used as a byte: `andi $aN, 0xFF` then keep in
  register, or `lbu $tN, spill+3` for each use. Which one IDO picks
  depends on whether `$aN` survives across a call between uses. If
  there is no call between two uses, the andi form is preferred. With
  a call between, the byte-load form. We can't always coerce IDO from
  the source side — note as a "tough scheduling" diff and revisit if a
  permuter exposes the trigger.
- **K&R f32 promotes to double.** K&R blocks emit `ldc1`/`cvt.s.d`
  around float args because K&R always promotes float to double.
  Switch any K&R with float args to ANSI prototype with `f32`. See
  `feedback_snap_kr_float_promotion.md`.
- **K&R int spill-elided when arg only used masked.** When the asm has
  `sw $aN, sp+homeN` for a K&R int that is only used as `arg & 0xFF`
  in a single call, plain K&R `int arg` will *not* emit the spill —
  IDO sees the arg is read-only as a masked byte and elides the home
  store. Force the spill by adding a dummy address-take: `s32 *home =
  &arg;`. This forces arg into memory at its home slot while leaving
  the call site as `func(...arg & 0xFF)` (`andi` in delay slot of
  `jal`). The unused `home` pointer compiles cleanly without an
  unused-var warning under `-Xfullwarn`. Confirmed on
  `func_8010A410` / `func_8010A450`.
- **ANSI int dead-spill via `arg ^ 0`.** When the asm has
  `sw $aN, sp+homeN` for an int arg that is otherwise pass-through to
  a single call (typical of "build matrix + apply matrix" wrappers
  where you can't use K&R because of mixed f32 args), plain ANSI
  declaration *won't* emit the spill. `s32 *home = &arg` works but
  bumps frame size by 8. Use `func(..., arg ^ 0, ...)` instead — XOR
  with zero is semantically a no-op but enough to force IDO into
  spilling the arg to its home slot. Permuter discovered this on
  `func_8011E25C` / `func_8011E40C`. Less Frankenstein-y than the
  `&arg` trick because it doesn't add a stack-allocated pointer var.
  Generalises across the whole "Mtx-build wrapper" family
  (`func_8011E25C/E40C` 6-arg, `func_8011B7FC/E08C` 10-arg,
  `func_8011BB48/D1F0` 11-arg). Suspect the original source had a
  pragma or wrapper macro doing equivalent forcing; `arg^0` is the
  minimal C source-side substitute.

### `bnel` / branch-likely loops
When the asm has a backward `bnel`/`bnezl`/`beql` whose delay slot is
the loop's update statement (e.g. `cur = cur->next`), the C source
must be **outer if + do-while-with-update-in-body**, not a plain
`while`:

```c
if (cond) {
    do {
        update;
    } while (cond);
}
```

Plain `while (cond) update;` makes IDO peel the loop differently
(extra forward checks, no `bnel`). See
`feedback_snap_bnel_do_while.md`, first confirmed match
`func_80100C74/D54` (sorted-list insert).

### Function-arg float in `$aN` register
A `f32` argument that follows a non-float arg is passed in the
**integer** register `$aN`, not `$f12/$f14`. IDO emits
`mtc1 $aN, $fM` to move it (bit-reinterpret, **not** a conversion).
Signature is `void f(s32 *p, f32 x)`, not `f(s32 *p, s32 x)` with
`(f32)x` cast — the cast emits `cvt.s.w` which doesn't match.

### `mul.s` operand-order quirk
`mul.s fd, fs, ft` is commutative but IDO's choice of which operand
goes in `fs` vs `ft` is hard to control from source. Most of the
time, the operand defined first ends up as `fs`. Sometimes the order
matches the C expression `a * b` → `fs=a, ft=b`. Sometimes it
doesn't. If a near-miss has only the `mul.s` instruction differing
by operand-order, defer it — this is decomp-permuter territory.

### Unconditional assign hoisted into branch delay slot
When the asm has a conditional branch whose **delay slot is a store
using the same register the branch tested** (e.g.
`beqz $a1, else; sw $a1, 0x8($a0)`), the C source has the assignment
**hoisted out** of the if-else as an unconditional statement before
the branch. IDO's scheduler swaps the unconditional store into the
delay slot:

```c
arg0->next = arg1;          /* unconditional — fills delay slot */
if (arg1 != 0) {
    /* then-branch updates arg0->prev via arg1->prev */
} else {
    /* else-branch updates arg0->prev via bucket head */
}
```

The trick works because the hoisted assignment is semantically
acceptable in both branches: when `arg1 == 0`, the store writes
NULL to the field, which is exactly what the else branch wants
anyway (a freshly-inserted tail-node has no `next`). Putting the
assignment inside the `if` instead causes IDO to emit `beql`
(branch likely) with the else-branch's first instruction in the
delay slot, shifting all subsequent bytes by one instruction.

Confirmed across the linked-list-insert family
(`func_8010848C`, `func_80108650`). Watch for this when the asm
prologue is `cond_branch $reg, label; sw $reg, offset($other)`
and the source-side guess of "if/else where each branch sets the
field differently" produces a `beql` mismatch.

### libultra `__os{Sp,Si,Dp}DeviceBusy`-style HW-bit-test (MATCHED)
Read a hardware status register, mask, return 1/0. Original allocates
an empty 8-byte stack frame and loads via `$a0`. Recipe:
```c
s32 func_xxx(void) {
    register u32 stat = *(volatile u32 *) 0xA40_____;
    if (stat & MASK) {
        return 1;
    } else {
        return 0;
    }
}
```
**Critical:** name the source file `src/ultra_os_<name>.c` to route it
through `CFLAGS_ULTRA_OS` (-O1). The project default `-O2` produces
a frameless `lui/lw $v0` form. The `register` keyword + `-O1` together
trigger the empty-frame + `lw $a0` shape that matches the original
libultra build. The function in `src/` keeps the `func_xxxxxxxx` name
— no symbol_addrs entry needed. Confirmed on `func_80137860`,
`func_80137890`, `func_8013CD50` (= `__osSpDeviceBusy`,
`__osSiDeviceBusy`, `__osDpDeviceBusy`).

Generalises to any other libultra `__os*Busy`-style helper sitting
inside the main segment. Look for: 1-arg-less HW reg read, mask,
boolean return, asm shows empty 8B frame + `$a0` as the temp + join-
merge layout `b end / addiu $v0,1 // .else: or $v0,0,0 / .end: jr $ra
addiu $sp,8`.

## "Tough nut" categories (defer until tooling helps)

- **No-CSE on duplicate global lui** (`func_80137700`). IDO normally
  CSEs two `lui %hi(SYM)` of the same symbol; the original has two
  separate `lui` for the same address. Likely the original source
  declares the symbol twice with different aliases.
- **Hoist-defeat on conditional getter** (`func_80138DB0`). IDO
  hoists `lui $v0` into the delay slot above the branch in our
  build; the original keeps `lui` *after* the branch. Volatile
  doesn't help (only applies to the `lw`, not the independent
  `lui`).
- **Mixed-call $a1 byte preservation** — RESOLVED. The functions
  `func_8010A8A4`, `func_8010A8E8`, and `func_8010AB30` all match
  with K&R `unsigned char arg1` and an `if (arg1 >= 0x20) { panic; }`
  guard before the body for AB30. The "byte preservation across call"
  pattern uses `sb $a1, 0x27($sp); ... lbu $a1, 0x27($sp)` from the
  K&R spill area when the byte needs to survive across a `jal`; the
  K&R declaration alone is enough to trigger this. Confirmed at -O2
  default flags.

## decomp-permuter usage

decomp-permuter is installed locally (gitignored) at
`tools/decomp-permuter/` and driven by the wrapper
`tools/permute_run.sh`. The permuter runs the project's exact IDO 7.1
matching CFLAGS against a candidate-set the wrapper builds from
splat-emitted asm and a hand-written seed C file.

Workflow:

1. Write a seed `*.c` somewhere (does NOT need to be in `src/`; can be
   `/tmp/seed_func_<addr>.c`). Include `"common.h"` and define exactly
   the function being matched.
2. Run:
   ```
   tools/permute_run.sh 0x80131A54 0x38 /tmp/seed_func_80131A54.c \
       -- -j 4 --speed 100 --stop-on-zero
   ```
   Args after `--` are passed straight through to `permuter.py`. Useful
   ones: `-j N` (parallel workers), `--speed N` (1..100, raises
   randomization rate), `--stop-on-zero` (exit when one matches),
   `--debug --print-diffs` (see the initial near-miss diff before
   randomization starts).
3. On match, the wrapper exits 0 and prints the path to a
   `runs/<func>/output-0-N/source.c` with a score-0 candidate. That
   file is the post-pycparser canonicalised form — typically with new
   tmp variables IDO didn't strictly need. Translate it back to a
   minimal hand-written form before committing to `src/`.

Validated end-to-end on `func_80131A54` (vram 0x80131A54, ROM 0x32654,
size 0x38). Initial near-miss at score 10 was a `mul.s` operand-order
diff; permuter found a score-0 permutation at iteration ~330 with
`--speed 100`.

Notes / pitfalls:

- Never commit anything under `tools/decomp-permuter/`. The directory
  itself is a local install; `runs/` contains gitignored intermediates.
- `--debug --print-diffs` only runs the base candidate once and exits;
  use this to inspect the starting near-miss without burning iterations.
- The permuter's score-0 candidate is post-pycparser pretty-printed
  C — the syntax (e.g. `(new_var2 = arg1 * 1.0f)`) is a hint about
  IDO's scheduling, not the literal source line that should land in
  the repo. Hand-rewrite it cleanly, recompile, confirm `match_check`
  still says MATCH, then commit.
- Permuter mutations are derived purely from the seed C the user
  wrote, so output is IP-clean by construction. Still discard any
  candidate with suspicious symbol names / magic constants pulled from
  somewhere unexplained — same rule as any other contribution.
- **The permuter cannot synthesise K&R-shaped codegen from an ANSI
  seed.** pycparser rejects K&R, so the seed is always ANSI; the
  permuter then mutates statements and locals but never re-introduces
  the K&R-style spill-all-args prologue. If your closest near-miss is
  a K&R variant (e.g. `unsigned char arg`) at N bytes diff, the
  permuter's plateau from any ANSI seed will likely be *much worse*
  than N bytes — the K&R prologue shape is below its mutation
  vocabulary. Confirmed on `func_80123074` (K&R `unsigned char arg1`
  → 4-byte regalloc swap; ANSI seed permuter plateaus at score 140
  after 28k iters). Strategy when this happens: don't burn permuter
  budget; instead try source-side flips on the K&R form
  (param-decl reorder, explicit `s32 saved = arg0`, struct-typed
  pointer cast) and live-debug.

## Ideas to invest in next

- **decomp-permuter.** Now installed (see above) — apply to deferred
  scheduling / operand-order / register-allocation near-misses. The
  "tough nut" category functions are the prime backlog.
- **Diff against a leaked IDO 7.1 sub-version.** If `ido-static-recomp`
  ships 7.1 patch level X and the ROM was built with patch level Y, a
  handful of scheduling decisions can flip. Worth verifying the exact
  sub-version against any of the open N64 SDK leaks.
- **Match libultra against a known libultra source tree.** The
  game's libultra was almost certainly compiled from the public N64
  SDK source; matching that range function-by-function unlocks the
  hardware-status helpers and the entire OS-API surface, lifting
  matched% by several points at once.
- **Per-file flag overrides.** Add Makefile target rules letting
  individual `.c` files compile with `-mips3` or alternate `-O`
  levels — needed for the 64-bit math runtime helpers and possibly
  for boot/libultra.
- **Cross-link a working `__ull_rem` implementation.** Low-effort
  alternative if we don't want per-file flag overrides for one or
  two helpers.
