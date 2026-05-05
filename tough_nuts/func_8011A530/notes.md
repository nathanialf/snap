# func_8011A530 — vec3 *= scalar (6/13 mul.s scheduling diff)

`v->x *= s; v->y *= s; v->z *= s; return v;`

## Diff (6/13)

Base inserts a `nop` between the second and third `mul.s` (probably
to avoid back-to-back FPU pipeline issue), then schedules all three
swc1 stores after the muls in order. Built emits the three muls
back-to-back without the nop, then the stores, and ends with a nop
after the delay-slot swc1.

## Permuter hint

Try splitting one of the multiplications across a function call or
adding a `volatile` so IDO is forced to emit the nop spacer.
