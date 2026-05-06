# func_80115448 — set 5 globals to D_8004B050 (lui at grouping)

Initialises D_8004B03C/_40/_44/_48/_4C all to point at D_8004B050.

## Diff (tooling-level — likely uncrackable from C)

Target uses 3 `lui $at` (one per pair-or-singleton group), reusing
$at across stores with the same %hi:

- `lui at, %hi(B03C); sw v0, %lo(B03C)(at)`
- `lui at, %hi(B040); sw v0, %lo(B040)(at); sw v0, %lo(B044)(at)`
- `lui at, %hi(B048); sw v0, %lo(B048)(at); ... ; sw v0, %lo(B04C)(at)`

Built emits 5 separate `lui $at` (one per store). `mips-linux-gnu-as`
doesn't combine adjacent same-hi pseudo-loads the way IDO's SGI `as`
did — IDO's frontend emits `sw $2, D_8004BXX` directly in the .s and
relies on the assembler's relaxation pass to share luis. That pass
isn't replicated by `ido-static-recomp`'s pipeline + GNU as.

## Variants tried
- Direct stores `D_8004B040 = p`: 5 luis (one per store)
- Array notation `extern u8 *D_8004B040[2]`: 4 luis (worse — base regs)
- `*(&base + n)` arithmetic: similar to array

## Verdict
Permuter can't crack this — lui-sharing is determined by the
assembler post-pass, not the C source. Park indefinitely.
