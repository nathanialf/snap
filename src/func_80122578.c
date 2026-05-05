#include "common.h"

extern void func_800288C0(s32, u8, u8);
extern s32 D_800968F0[];

void func_80122578(s32 arg0, s32 arg1) {
    s32 i;
    if ((u32)arg1 >= 0x80) {
        arg1 = 0x7F;
    }
    for (i = 0; i < 16; i++) {
        func_800288C0(D_800968F0[arg0], (u8)i, (u8)arg1);
    }
}
