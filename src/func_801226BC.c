#include "common.h"

extern s32 *D_800968F0[];

extern void func_8002891C(s32 *, s8);
extern void func_8002E0CC(s32 *, u8, s8);
extern void func_8002DDE0(s32 *);
extern void func_8002A390(s32 *);

void func_801226BC(arg0, arg1)
s32 arg0;
s8 arg1;
{
    func_8002891C(D_800968F0[arg0], arg1);
}

void func_801226F4(arg0, arg1, arg2)
s32 arg0;
u8 arg1;
s8 arg2;
{
    func_8002E0CC(D_800968F0[arg0], arg1, arg2);
}

void func_80122734(s32 arg0) {
    func_8002DDE0(D_800968F0[arg0]);
}

void func_80122760(s32 arg0) {
    func_8002A390(D_800968F0[arg0]);
}
