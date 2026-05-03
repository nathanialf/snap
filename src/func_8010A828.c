#include "common.h"

extern u8 D_800434E8[];
extern s32 D_80040CF8;
extern void func_80025F24(u8 *, s32, s32);

void func_8010A828(arg0, arg1, arg2, arg3, arg4)
u8 *arg0;
s32 arg1;
u8 arg2;
s32 arg3;
s32 arg4;
{
    if (arg2 >= 0x20) {
        func_80025F24(D_800434E8, arg2, *(s32 *) arg0);
        for (;;) {}
    }
    arg0[0xD] = arg2;
    *(s32 *) (arg0 + 0x28) = arg3;
    *(s32 *) (arg0 + 0x2C) = arg1;
    *(s32 *) (arg0 + 0x34) = arg4;
    arg0[0xE] = D_80040CF8 - 1;
}
