# func_801158E8 — squared distance (mul/add 2-insn swap)

`return dx*dx + dy*dy + dz*dz` between arg0->{_1C,_20,_24} and
D_8004AC48->_48->{_3C,_40,_44}.

## Diff (2/18)

Just the mul.s f10,f14,f14 (= dz²) and add.s f8,f4,f6 (= dx²+dy²)
order is swapped — both ops are independent and IDO chose the
reverse order.

## Permuter hint

Try parens variations: `(dx*dx + dy*dy) + dz*dz` (no help),
explicit local for dz² before the final add.
