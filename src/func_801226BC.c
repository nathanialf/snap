#include "common.h"

extern s32 *D_800968F0[];
extern void func_8002891C(s32 *, s8);

void func_801226BC(arg0, arg1)
s32 arg0;
s8 arg1;
{
    func_8002891C(D_800968F0[arg0], arg1);
}
