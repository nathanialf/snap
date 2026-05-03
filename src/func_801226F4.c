#include "common.h"

extern s32 *D_800968F0[];
extern void func_8002E0CC(s32 *, u8, s8);

void func_801226F4(arg0, arg1, arg2)
s32 arg0;
u8 arg1;
s8 arg2;
{
    func_8002E0CC(D_800968F0[arg0], arg1, arg2);
}
