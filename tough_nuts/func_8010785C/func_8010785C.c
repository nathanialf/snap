#include "common.h"

extern u8 D_80043260[];

extern void func_80025F24(u8 *, s32);

s32 func_8010785C(s32 *arg0, s32 arg1, s32 arg2) {
    s32 v1;

    if (arg2 != 0) {
        v1 = (arg0[3] + (arg2 - 1)) & ~(arg2 - 1);
    } else {
        v1 = arg0[3];
    }
    arg0[3] = v1 + arg1;
    if (arg0[2] < arg0[3]) {
        func_80025F24(D_80043260, arg0[0]);
        while (1) {}
    }
    return v1;
}
