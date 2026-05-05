#include "common.h"

extern void func_80033BE0(s32, s32);

void func_801078C4(arg0, arg1, arg2, arg3)
s32 *arg0;
s32 arg1;
s32 arg2;
s32 arg3;
{
    arg0[0] = arg1;
    arg0[3] = arg2;
    arg0[1] = arg2;
    arg0[2] = arg2 + arg3;
    func_80033BE0(arg2, arg3);
}
