#include "common.h"

u32 func_80132044(s32 *arg0) {
    u8 *p = (u8 *) arg0[2];
    u8 *q;
    u32 b0, b1, b2, b3;

    q = p + 1;
    b0 = *p;
    arg0[2] = (s32) q;
    b1 = *q;
    p = q + 1;
    arg0[2] = (s32) p;
    b2 = *p;
    q = p + 1;
    arg0[2] = (s32) q;
    b3 = *q;
    p = q + 1;
    arg0[2] = (s32) p;
    return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}
