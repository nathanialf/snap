#include "common.h"

extern s32 D_800488C0;
extern s32 D_800488C4;
extern s32 D_800488C8;
extern s32 D_80003478;

extern void func_80002C20(s32, s32, s32);
extern void func_8000345C(s32, s32, s32);
extern void func_80002C94(s32, s32, void *, s32);
extern void func_800034C4(s32, s32, u8 *, s32);

void func_8010345C(s32 arg0, s32 arg1, s32 arg2) {
    D_800488C8 = arg0;
    D_800488C0 = arg1;
    D_800488C4 = arg2;
}

void func_80103478(void) {
    func_80002C20(D_800488C8, D_800488C0, D_800488C4);
    D_800488C8 += D_800488C4;
}

void func_801034C4(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_8000345C(arg0, arg2, arg3);
    func_80002C94(arg2, arg3, &D_80003478, arg1);
}

void func_8010350C(s32 arg0, s32 arg1) {
    u8 buf[1024];
    func_800034C4(arg0, arg1, buf, 0x400);
}
