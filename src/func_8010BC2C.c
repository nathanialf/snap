#include "common.h"

extern u8 D_8000BC0C;

extern void func_8000BAA4(s32, u8 *, s32, s32);
extern void func_8000BB4C(u8 *, s32, s32);
extern void func_8000B880(s32);

void func_8010BC2C(s32 arg0, s32 arg1) {
    func_8000BAA4(arg0, &D_8000BC0C, arg1, 1);
}

void func_8010BC58(s32 arg0) {
    func_8000BB4C(&D_8000BC0C, arg0, 1);
}

void func_8010BC84(arg0)
s32 arg0;
{
    func_8000B880(0);
}
