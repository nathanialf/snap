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

## "Tough nut" categories (defer until tooling helps)

- **libultra-style `$a0`-dest hardware-status checks** with empty 8B
  stack frame (`func_80137860/890/8013CD50`). Likely a different IDO
  version or pragma. Empty `addiu $sp,-8/+8` with no spills suggests
  an inline helper from a header.
- **No-CSE on duplicate global lui** (`func_80137700`). IDO normally
  CSEs two `lui %hi(SYM)` of the same symbol; the original has two
  separate `lui` for the same address. Likely the original source
  declares the symbol twice with different aliases.
- **Hoist-defeat on conditional getter** (`func_80138DB0`). IDO
  hoists `lui $v0` into the delay slot above the branch in our
  build; the original keeps `lui` *after* the branch. Volatile
  doesn't help (only applies to the `lw`, not the independent
  `lui`).
- **Mixed-call $a1 byte preservation** (`func_8010A8A4/A8E8/AB30`).
  K&R `u8 arg` with the byte used both before and across a function
  call — IDO chooses register-spill via `sb+lbu` (base) vs
  `lw+sw` of the masked value (built). No source-side trigger
  found; permuter territory.

## Ideas to invest in next

- **decomp-permuter.** The standard N64 decomp tool for resolving
  scheduling/operand-order/register-allocation near-misses. Mutates
  source within a candidate set and recompiles repeatedly; reports
  any permutation that matches. Most of the deferred functions in
  the "tough nut" categories above are exactly its use case.
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
