#include "common.h"

extern s32 D_80097D40;
extern s32 D_80097D44;
extern s32 D_80097D48;
extern s32 D_80097D4C;

void func_80126170(s32 *arg0, s32 arg1) {
    *(u16 *) ((u8 *) arg0 + 0x14) |= arg1;
}

void func_80126180(s32 *arg0, s32 arg1) {
    *(s16 *) ((u8 *) arg0 + 0x16) = arg1;
}

void func_80126188(s32 *arg0, s32 arg1, s32 arg2) {
    *(s16 *) ((u8 *) arg0 + 0x0) = arg1;
    *(s16 *) ((u8 *) arg0 + 0x2) = arg2;
}

void func_80126194(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    D_80097D48 = arg0;
    D_80097D4C = arg2;
    D_80097D40 = arg1;
    D_80097D44 = arg3;
}
