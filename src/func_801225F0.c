#include "common.h"

extern void func_80028970(s32, u8, u8);
extern s32 D_800968F0[];
extern u8 D_80096900;

void func_801225F0(s32 arg0, u8 arg1) {
    s32 i;
    if (arg1 >= 0x80) {
        arg1 = 0x7F;
    }
    D_80096900 = arg1;
    for (i = 0; i < 16; i++) {
        func_80028970(D_800968F0[arg0], (u8)i, arg1);
    }
}
