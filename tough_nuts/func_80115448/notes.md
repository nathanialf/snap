# func_80115448 — set 5 globals to D_8004B050 (lui at grouping)

Initialises D_8004B03C/_40/_44/_48/_4C all to point at D_8004B050.

## Diff

Body matches except `lui at` placement. Base groups _40+_44 under
one lui at and _48+_4C under another. Built groups differently,
producing 5/11 byte differences but functionally identical.

## Permuter hint

Try ordering the 5 stores in different ways or using struct
syntax to nudge IDO to combine the same set as base.
