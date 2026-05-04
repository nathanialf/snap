#include "common.h"

extern void func_800363C8(s32 *, s32);
extern void func_80036520(s32 *, s32 *, s32 *);
extern void func_80036210(s32 *, s32);

void func_801361B0(arg0, arg1, arg2)
s32 arg0;
s32 arg1;
s32 arg2;
{
    s32 buf1[16];
    s32 buf2[16];
    s32 buf3[16];
    func_800363C8(buf1, arg0);
    func_800363C8(buf2, arg1);
    func_80036520(buf1, buf2, buf3);
    func_80036210(buf3, arg2);
}
