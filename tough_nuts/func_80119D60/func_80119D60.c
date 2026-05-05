#include "common.h"

extern s32 D_80040F40;

s32 func_80119D60(void) {
    s32 v = D_80040F40 * 214013 + 0x269EC3;
    s32 ret = (v >> 16) & 0xFFFF;
    D_80040F40 = v;
    return ret;
}
