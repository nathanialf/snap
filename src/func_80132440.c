#include "common.h"

s32 func_80132440(arg0, arg1, arg2)
s32 *arg0;
s16 arg1;
u8  arg2;
{
    s32 *p = (s32 *) arg0[0x10];
    *(s16 *) ((u8 *) p + arg1 * 0x30 + 0x20) = arg2;
    return (s32) p;
}
