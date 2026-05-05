#include "common.h"

u32 func_80132044(s32 *arg0) {
    u8 *p = (u8 *) arg0[2];
    u32 b0;
    u32 b1;
    u32 b2;
    u32 b3;

    b0 = *p;
    p += 1;
    arg0[2] = (s32) p;
    b1 = *p;
    p += 1;
    arg0[2] = (s32) p;
    b2 = *p;
    p += 1;
    arg0[2] = (s32) p;
    b3 = *p;
    p += 1;
    arg0[2] = (s32) p;
    return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}
