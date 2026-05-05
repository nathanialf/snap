#include "common.h"

extern s32 D_8004A968[3];

extern void func_800009A0(s32 *);

void func_80107964(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp18[12];

    sp18[0] = 5;
    sp18[1] = 0x64;
    sp18[9] = arg0;
    sp18[10] = arg1;
    sp18[11] = arg2;
    D_8004A968[0] = arg0;
    D_8004A968[1] = arg1;
    D_8004A968[2] = arg2;
    func_800009A0(sp18);
}
