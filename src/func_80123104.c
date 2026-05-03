#include "common.h"

extern s32 D_800423B8;
extern s32 D_800423C8;

void func_801230DC(void) {
    D_800423B8 = 1;
}

s32 func_801230EC(void) {
    return D_800423B8;
}

s32 func_801230F8(void) {
    return D_800423C8;
}

s32 func_80123104(void) {
    return D_800423C8 | D_800423B8;
}
