#include "common.h"

void func_8012DE50(s32 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    arg0[0] = 0;
    arg0[1] = arg1;
    arg0[2] = arg2;
    *(s16 *) ((u8 *) arg0 + 0xC) = 0;
    *(s16 *) ((u8 *) arg0 + 0xE) = 0;
    arg0[4] = arg3;
}
