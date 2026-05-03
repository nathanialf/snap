#include "common.h"

extern s32 D_8004A890;
extern s32 D_8004A894;
extern s32 D_8004A898;
extern s32 D_8004A89C;
extern void func_80019158(s32, s32 *, s32);

void func_801191D8(s32 arg0) {
    func_80019158(arg0, &D_8004A890, 0);
}

void func_80119200(s32 arg0) {
    func_80019158(arg0, &D_8004A894, 1);
}

void func_80119228(s32 arg0) {
    func_80019158(arg0, &D_8004A898, 2);
}

void func_80119250(s32 arg0) {
    func_80019158(arg0, &D_8004A89C, 3);
}
