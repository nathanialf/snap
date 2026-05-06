#include "common.h"

extern s32 D_80040F40;

s32 func_80119D60(void) {
    return (D_80040F40 = D_80040F40 * 214013 + 0x269EC3, (D_80040F40 >> 16) & 0xFFFF);
}
