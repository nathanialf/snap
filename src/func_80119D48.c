#include "common.h"

extern s32 D_80040F40;

void func_80119D48(s32 arg0) {
    D_80040F40 = arg0;
}

s32 func_80119D54(void) {
    return D_80040F40;
}
