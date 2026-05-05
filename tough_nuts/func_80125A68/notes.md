# func_80125A68 — varargs wrapper passing &arg1 (va_list)

Looks like a varargs wrapper. The asm computes
`a1 = (sp + 0x27) & -4` — the va_start/va_list alignment trick to
get a 4-byte-aligned pointer to the first variadic arg. Then calls
func_80025938(a0, ap, a2, a3).

## Diff

Built doesn't reproduce the (sp + 0x27) & -4 alignment pattern.
Plain `&arg1` doesn't trigger it; K&R doesn't either.

## Permuter hint

Try `<stdarg.h>` with `va_list ap; va_start(ap, arg0)` if the
project gains a stdarg shim. Otherwise this needs a custom
inline-asm trick or a permuter with an explicit alignment hint.
