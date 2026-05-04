#include "common.h"

extern s32 D_80040F40;

f32 func_80119DB0(void) {
    D_80040F40 = D_80040F40 * 214013 + 2531011;
    return (D_80040F40 >> 16 & 0xFFFF) / (f32) 0x10000;
}
