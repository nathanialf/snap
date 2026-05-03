#include "common.h"

extern s32 D_800488A0;
extern void func_80002A10(s32, s32, s32, s32, s32);

void func_80102C20(arg0, arg1, arg2)
s32 arg0;
s32 arg1;
s32 arg2;
{
    func_80002A10(D_800488A0, arg0, arg1, arg2, 0);
}

void func_80102C5C(arg0, arg1, arg2)
s32 arg0;
s32 arg1;
s32 arg2;
{
    func_80002A10(D_800488A0, arg1, arg0, arg2, 1);
}
