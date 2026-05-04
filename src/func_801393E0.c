#include "common.h"

extern s32 func_8003B530(s32, s32, s32, void *, s32);

s32 func_801393E0(s32 *arg0, u8 arg1) {
    u8 buf[32];
    s32 i;
    s32 result;

    for (i = 0; i < 32; i++) {
        buf[i] = arg1;
    }
    result = func_8003B530(arg0[1], arg0[2], 0x400, buf, 0);
    if (result == 0) {
        ((u8 *) arg0)[0x65] = arg1;
    }
    return result;
}
