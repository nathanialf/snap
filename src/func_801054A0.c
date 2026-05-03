#include "common.h"

extern s32 D_8004A8C8;
extern s32 func_8000785C(s32 *, s32, s32);

void func_801054A0(arg0, arg1)
s32 arg0;
s32 arg1;
{
    func_8000785C(&D_8004A8C8, arg0, arg1);
}
