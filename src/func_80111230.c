#include "common.h"

extern s32 D_80040F30;
extern s32 D_80040F34;
extern s32 D_80040F38;
extern s32 D_80040F3C;
extern s32 D_8004B038;

void func_80111230(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    D_80040F30 = arg0;
    D_80040F34 = arg1;
    D_80040F38 = arg2;
    D_80040F3C = arg3;
}

void func_80111254(s32 arg0) {
    D_8004B038 = arg0;
}

void func_80111260(void) {
}
