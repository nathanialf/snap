#include "common.h"

extern f32 D_800439F0;

void func_8011054C(u8 *arg0, s32 arg1, f32 arg2) {
    u8 *p = *(u8 **) (arg0 + 0x6C);
    while (p != 0) {
        p[0x5] = 0;
        p = *(u8 **) p;
    }
    *(s32 *) (arg0 + 0x70) = arg1;
    *(f32 *) (arg0 + 0x74) = D_800439F0;
    *(f32 *) (arg0 + 0x7C) = arg2;
}
