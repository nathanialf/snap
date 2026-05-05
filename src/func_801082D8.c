#include "common.h"

extern s32 *D_8004AC50;
extern s32 D_8004A9A8;

s32 func_801082D8(void) {
    return (s32) D_8004AC50;
}

s32 func_801082E4(s32 *arg0) {
    s32 r = 0;
    if (arg0 == 0) {
        arg0 = D_8004AC50;
    }
    if (arg0 != 0 && *(u8 *) ((u8 *) arg0 + 0x14) == 0) {
        r = *(s32 *) ((u8 *) (*(s32 **) ((u8 *) arg0 + 0x1C)) + 0x1B8);
        return r;
    }
    return r;
}

s32 func_8010831C(s32 *arg0) {
    s32 r = 0;
    if (arg0 == 0) {
        arg0 = D_8004AC50;
    }
    if (arg0 != 0 && *(u8 *) ((u8 *) arg0 + 0x14) == 0) {
        r = *(s32 *) ((u8 *) (*(s32 **) ((u8 *) arg0 + 0x1C)) + 0x1BC);
        return r;
    }
    return r;
}

void func_80108354(s32 arg0) {
    D_8004A9A8 = arg0;
}
