# func_80100870 — game-side init with two 64-bit FEDCBA98 magic stores

Initialises two pairs of words (D_800456A8/AC and D_80045A58/5C) to
the 64-bit value 0x00000000_FEDCBA98 (low word = 0, high word
0xFEDCBA98), interleaved with calls to func_8003D8A0(0x04900000),
func_80034E30, func_80034A00 (6-arg), and func_8003D0F0.

## What's matching

All of the function calls match in argument setup. The diffs are
clustered around the two pairs of magic-value stores.

## Diff

Base treats each pair as a single 64-bit store: load 0 into a t-reg,
load 0xFEDCBA98 into another t-reg, then ONE `lui at, hi(addr)`
followed by two `sw` (one at +0, one at +4) reusing the same `at`.

Built code (all variants tried) emits each store independently with
its own `lui at`, so two LUIs per pair, and the scheduling drifts.

## Variants tried

- `D_800456A8 = 0; D_800456AC = 0xFEDCBA98;` — uses `sw $0`, two LUIs
- `extern u64 D_800456A8; = 0xFEDCBA98ULL;` — uses t-reg for 0 ✓ but
  two LUIs ✗ and prologue scheduling drifted
- (Volatile variants not tried yet — should permuter.)

## Permuter hint

Try wrapping the pair stores in a tiny static helper that takes the
base address and the magic value, so IDO inlines a single
lui-at-share pattern. Or try `__osSetTime`-style libultra inlining
if these turn out to map to libultra `__osCurrentTime` / similar
(magic 0xFEDCBA98 is the libultra time-init sentinel).
