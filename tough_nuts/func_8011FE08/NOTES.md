# func_8011FE08 — relocate slot pointers in header

```c
if (arg0->_2 > 0) {
    s32 *p = (s32 *)arg0;
    s32 i = 0;
    do {
        void *r = (void *)(p[1] + (s32)arg0);
        p[1] = (s32)r;
        if (r != 0) func_8001FE88(r, arg0, arg1);
        i++;
        p++;
    } while (i < arg0->_2);
}
```

## Diff: s1/s2 swap for arg0 vs counter

Target:
- s1 = arg0 (base pointer, preserved)
- s2 = i (loop counter)

Mine (after fixing the duplicate-blez issue):
- s2 = arg0 (swapped!)
- s1 = i (swapped!)

Same instruction count, identical layout otherwise. The s-register
assignment for which-saves-which is just IDO's reg-alloc choice.

## Tried

- Swapping declaration order of `i` and `p` — no effect.
- Doing the ANSI-style early-return + for-loop got DOUBLE blez
  (one for early return, one for for-loop top). Switching to
  `if (cond) { do ... while }` form fixed that.

## Try later

- Permuter for s-reg assignment permutation
- Add a redundant intermediate variable that might shift the
  liveness analysis
