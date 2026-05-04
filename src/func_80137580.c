#include "common.h"

extern s32 D_80042D10;

extern s32 func_80038DB0(void);
extern s32 func_800378C0(s32, void *, s32);
extern s32 func_80032630(s32, void *, s32);

s32 func_80137580(arg0, arg1, arg2, arg3)
s32 *arg0;
s16 *arg1;
s32 arg2;
s32 arg3;
{
    s32 saveMask;
    s32 result;
    if (D_80042D10 == 0) {
        return -1;
    }
    *(s32 *) ((u8 *) arg1 + 0x14) = (s32) arg0;
    arg1[0] = (arg2 != 0) ? 0x10 : 0xF;
    saveMask = func_80038DB0();
    if (((u8 *) arg1)[2] == 1) {
        result = func_800378C0(saveMask, arg1, 0);
    } else {
        result = func_80032630(saveMask, arg1, 0);
    }
    return result;
}
