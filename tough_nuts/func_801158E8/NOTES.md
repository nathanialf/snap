# func_801158E8 — squared-distance leaf

Simple leaf: returns squared distance between arg0->_1C/_20/_24 and
`D_8004AC48->_48`'s _3C/_40/_44 (Bar struct).

## Diff: scheduling of dz² mul vs (dx²+dy²) add

Target schedules `add.s f8, f4, f6` (dx²+dy²) BEFORE the third mul:

```
mul.s f6, f12, f12     ; dy²
add.s f8, f4, f6       ; dx² + dy²    ← target adds here
mul.s f10, f14, f14    ; dz²
jr ra
add.s f0, f8, f10
```

Mine schedules dz² mul before the add:

```
mul.s f6, f12, f12     ; dy²
mul.s f10, f14, f14    ; dz²          ← mine multiplies here
add.s f8, f4, f6       ; dx² + dy²
jr ra
add.s f0, f8, f10
```

Same instructions, different order. IDO -O2 scheduler picked differently;
likely register-pressure-based heuristic. Both are valid.

Tried `(dx*dx + dy*dy) + dz*dz` and `f32 d2 = dx*dx + dy*dy; return d2 + dz*dz;`
— neither moves the `add` ahead of the third mul. Probably needs permuter.

## Try later

- decomp-permuter for scheduling permutations
- Different struct layout that changes load latency analysis
