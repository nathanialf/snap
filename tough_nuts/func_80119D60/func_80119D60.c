#include "common.h"

extern s32 D_80040F40;

s32 func_80119D60(void) {
    s32 t = D_80040F40;
    s32 r = t * 4 - t;
    r = r * 4 + t;
    r = r * 16 + t;
    r = r * 256 - t;
    r = r * 4 + t;
    r += 0x269EC3;
    D_80040F40 = r;
    return (r >> 16) & 0xFFFF;
}
