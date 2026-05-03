#include "common.h"

extern s32 D_80040CF8;

void func_8010A9D8(u8 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    arg0[0xD] = 0x20;
    *(s32 *) (arg0 + 0x28) = arg2;
    *(s32 *) (arg0 + 0x2C) = arg1;
    *(s32 *) (arg0 + 0x30) = arg3;
    *(s32 *) (arg0 + 0x34) = arg4;
    *(s32 *) (arg0 + 0x38) = 0;
    arg0[0xE] = D_80040CF8 - 1;
}
