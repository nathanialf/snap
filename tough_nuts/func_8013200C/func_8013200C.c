#include "common.h"

s16 func_8013200C(s32 *arg0) {
    u8 *p = (u8 *) arg0[2];
    s16 v = (s16) (*p << 8);
    arg0[2] = (s32) (p + 1);
    v |= p[1];
    arg0[2] = (s32) (p + 2);
    return v;
}
