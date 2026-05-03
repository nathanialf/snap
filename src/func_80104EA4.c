#include "common.h"

extern u8 D_80048C33;
extern u8 D_800489B8;
extern void func_80004D94(s32, s32, s32);
extern void func_80032840(u8 *, s32 *, s32);

u8 func_80104EA4(void) {
    s32 local;
    func_80004D94(3, 0, 0);
    func_80032840(&D_800489B8, &local, 1);
    return D_80048C33;
}

void func_80104EE4(arg0)
u8 arg0;
{
    s32 local;
    func_80004D94(3, 1, arg0);
    func_80032840(&D_800489B8, &local, 1);
}
