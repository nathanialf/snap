#include "common.h"

extern void func_800363C8(s32 *, s32);
extern void func_80036480(s32 *, s32, s32, s32, s32, s32, s32);

void func_80136150(arg0, arg1, arg2, arg3, arg4, arg5, arg6)
s32 arg0;
s32 arg1;
s32 arg2;
s32 arg3;
s32 arg4;
s32 arg5;
s32 arg6;
{
    s32 buf[16];
    func_800363C8(buf, arg0);
    func_80036480(buf, arg1, arg2, arg3, arg4, arg5, arg6);
}
