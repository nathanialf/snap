# func_80119C40 — acos clamp (sister of func_80119BBC)

`return D_80043D84 - func_80019994(arg0 / func_80036090(1.0f - arg0*arg0));`
with the same 2-clamp early-return pattern as func_80119BBC.

## Diff (2/36)

Same bc1f vs bc1fl + mul.s scheduling diff as the asin sibling
func_80119BBC. Body is correct.

## Permuter hint

Same family as func_80119BBC. Cracking that should also crack this.
