#include "common.h"

extern f32 D_80043750;
extern f32 D_80043754;

void func_8010C7CC(u8 *arg0, s32 arg1, f32 arg2) {
    u8 *p = *(u8 **) (arg0 + 0x6C);
    while (p != 0) {
        p[0x5] = 0;
        p = *(u8 **) p;
    }
    *(s32 *) (arg0 + 0x70) = arg1;
    *(f32 *) (arg0 + 0x74) = D_80043750;
    *(f32 *) (arg0 + 0x7C) = arg2;
}

void func_8010C804(u8 *arg0, s32 arg1, f32 arg2) {
    u8 *p = *(u8 **) (arg0 + 0x90);
    while (p != 0) {
        p[0x5] = 0;
        p = *(u8 **) p;
    }
    *(s32 *) (arg0 + 0x94) = arg1;
    *(f32 *) (arg0 + 0x98) = D_80043754;
    *(f32 *) (arg0 + 0xA0) = arg2;
}
