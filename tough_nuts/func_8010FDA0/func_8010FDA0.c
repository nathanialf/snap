#include "common.h"

extern void func_80009760(s32 *, s32, s32);

void func_8010FDA0(arg0, arg1, arg2, arg3)
s32 *arg0;
u8 arg1;
u8 arg2;
u8 arg3;
{
    if (arg1 != 0) {
        func_80009760(arg0, arg1, 0);
    }
    if (arg2 != 0) {
        func_80009760(arg0, arg2, 0);
    }
    if (arg3 != 0) {
        func_80009760(arg0, arg3, 0);
    }
}
