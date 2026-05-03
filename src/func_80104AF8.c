#include "common.h"

extern void func_80004A24(s32, s32, u16, s32, u16);

void func_80104AF8(arg0, arg1, arg2, arg3)
s32 arg0;
u16 arg1;
s32 arg2;
u16 arg3;
{
    func_80004A24(arg0, 0, arg1, arg2, arg3);
}

void func_80104B34(arg0, arg1, arg2, arg3)
s32 arg0;
u16 arg1;
s32 arg2;
u16 arg3;
{
    func_80004A24(arg0, 1, arg1, arg2, arg3);
}
