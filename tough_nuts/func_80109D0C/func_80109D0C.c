#include "common.h"

extern f32 D_800436F8;

void func_80109D0C(s32 *arg0) {
    register s32 i;
    s32 *p;
    i = 1;
    arg0[0x13] = 0;
    *(u8 *) ((u8 *) arg0 + 0x54) = 0;
    *(u8 *) ((u8 *) arg0 + 0x55) = 0;
    *(u8 *) ((u8 *) arg0 + 0x56) = 0;
    arg0[0x16] = 0;
    p = arg0 + i;
    p[0x17] = 0;
    p[0x18] = 0;
    p[0x19] = 0;
    p[0x16] = 0;
    arg0[0x1B] = 0;
    arg0[0x1C] = 0;
    *(f32 *) ((u8 *) arg0 + 0x74) = D_800436F8;
    *(f32 *) ((u8 *) arg0 + 0x78) = 1.0f;
    *(f32 *) ((u8 *) arg0 + 0x7C) = 0.0f;
    arg0[0x20] = 0;
    arg0[0x21] = 0;
}
