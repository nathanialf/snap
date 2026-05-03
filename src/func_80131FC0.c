#include "common.h"

void func_80131FC0(s32 *arg0, s32 *arg1) {
    *(s32 *) ((u8 *) arg0 + 0x8) = *(s32 *) ((u8 *) arg1);
    *(s16 *) ((u8 *) arg0 + 0x1A) = *(s16 *) ((u8 *) arg1 + 0xC);
    *(s32 *) ((u8 *) arg0 + 0xC) = *(s32 *) ((u8 *) arg1 + 0x4);
}

void func_80131FDC(s32 *arg0, s32 *arg1) {
    *(s32 *) ((u8 *) arg1) = *(s32 *) ((u8 *) arg0 + 0x8);
    *(s16 *) ((u8 *) arg1 + 0xC) = *(s16 *) ((u8 *) arg0 + 0x1A);
    *(s32 *) ((u8 *) arg1 + 0x4) = *(s32 *) ((u8 *) arg0 + 0xC);
}

