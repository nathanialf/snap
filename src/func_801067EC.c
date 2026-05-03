#include "common.h"

extern s32 D_8004A8B0;
extern s32 D_8004A8B4;

void func_801067DC(void) {
    D_8004A8B0 = 1;
}

void func_801067EC(s32 arg0) {
    D_8004A8B0 = 2;
    D_8004A8B4 = arg0;
}
