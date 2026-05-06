#include "common.h"

extern s32 D_8004A8B0;
extern s32 D_8004A8B4;
extern s32 D_80048814;
extern s32 D_8004979C;
extern u8 D_800497A8;

extern void func_800009A0(void *);
extern s32 func_80032840(u8 *, s32, s32);

void func_801067DC(void) {
    D_8004A8B0 = 1;
}

void func_801067EC(s32 arg0) {
    D_8004A8B0 = 2;
    D_8004A8B4 = arg0;
}

s32 func_80106804(void) {
    s32 buf[9];
    switch (D_8004A8B0) {
        case 1:
            return 1;
        case 2:
            if (D_80048814 != 0) {
                buf[0] = 0xB;
                buf[1] = 0x64;
                func_800009A0(buf);
                return 1;
            }
            return 0;
    }
    return 0;
}

void func_80106878(void) {
    if (D_8004979C == 1) {
        D_8004979C = 2;
        while (func_80032840(&D_800497A8, 0, 0) != -1) {
        }
        func_80032840(&D_800497A8, 0, 1);
        D_8004979C = 0;
    }
}
