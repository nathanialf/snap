# func_801314EC — return-with-fallback (saturate-to-default)

**ROM offset:** `0x320EC` **size:** `0x24` (9 instructions)
**VRAM:** `0x801314EC`

## Behaviour
```c
s32 d = arg0[9] - arg1;
return (d < 0) ? 0x3E8 : d;
```

If positive: return d. If negative: return 0x3E8.

## What's matching
- Layout/control flow correct
- Pre-load of `0x3E8` happens before subu (matches base scheduling)

## Remaining diff (3 differing instructions)
Pure register-allocation swap: base puts `result` (0x3E8) in `$v0`
and `diff` in `$v1`. Built puts them swapped (`$v1` for result,
`$v0` for diff).

```
base                              built
  li   $v0, 0x3E8                   li   $v1, 0x3E8
  subu $v1, $t6, $a1                subu $v0, $t6, $a1
  bltz $v1, end                     bltz $v0, +2
  nop                               nop
  jr   $ra                          or   $v1, $v0, $zero
  or   $v0, $v1, $zero              jr   $ra
end: jr $ra                         or   $v0, $v1, $zero
     nop
```

The `register` keyword on `result` didn't force $v0 allocation.

## Variants tried
- `if (d < 0) return 0x3E8; return d;` → `bgez` instead of `bltz`,
  layout reversed.
- `return (d >= 0) ? d : 0x3E8;` ternary → similar regalloc swap.
- `register s32 result = 0x3E8;` hint → no change.

## Permuter hint
Trivial regalloc swap. Permuter should crack this in seconds.
Source body is correct; just need IDO to keep `result` in the
return register from the start.
