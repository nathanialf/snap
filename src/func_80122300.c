#include "common.h"

extern s32 D_800423C4;
extern s32 D_800423C8;
extern u8 D_80096960;
extern void func_800223F8(s32);

void func_80122300(s32 arg0) {
    if (arg0 != D_800423C4) {
        D_800423C8++;
        D_80096960 = arg0;
    }
}

void func_80122334(void) {
    s32 i;
    for (i = 0; i != 2; i++) {
        func_800223F8(i);
    }
}
