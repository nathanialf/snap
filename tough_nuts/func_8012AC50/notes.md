# func_8012AC50 — round-half-up float-to-int (fp-reg routing)

`(s32)(f32)((f64)((f32)arg1 * (f32)arg0->_44) / D_800442B0 + 0.5)`.
Math is correct but mtc1 chooses different fp regs (f8 vs f4).

## Permuter hint

Try splitting the expression into sub-locals to coax IDO into the
base's fp-reg choices.
