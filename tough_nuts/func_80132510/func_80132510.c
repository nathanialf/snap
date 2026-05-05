#include "common.h"

s32 func_80132510(s32 *arg0, u8 *arg1) {
    s32 cap = arg0[0x11];
    u8 *base = (u8 *) arg0[0x10];
    s16 i;

    if (cap > 0) {
        for (i = 0; i < cap; i++) {
            u8 *e = base + i * 48;
            if (*(void **) (e + 0x1C) == 0) {
                *(void **) (e + 0x1C) = arg1;
                *(s16 *) (e + 0x20) = 5;
                *(s32 *) (e + 0x28) = 0;
                *(u8 *) (e + 0x2E) = 0x40;
                *(u8 *) (e + 0x2F) = 0;
                *(f32 *) (e + 0x24) = 1.0f;
                *(s16 *) (e + 0x2C) = (arg1[0xD] * 0x7FFF) >> 7;
                return i;
            }
        }
    }
    return -1;
}
