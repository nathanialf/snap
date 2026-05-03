#include "common.h"

extern void func_8000A5F8(s32, s32, u8, s32, s32);

void func_8010A740(arg0, arg1, arg2)
s32 arg0;
u8 arg1;
s32 arg2;
{
    func_8000A5F8(0, arg0, arg1, arg2, 0);
}

void func_8010A778(arg0, arg1, arg2)
s32 arg0;
u8 arg1;
s32 arg2;
{
    func_8000A5F8(1, arg0, arg1, arg2, 0);
}

void func_8010A7B0(s32 arg0, u8 *arg1) {
    func_8000A5F8(2, arg0, arg1[0xC], *(s32 *) (arg1 + 0x10), (s32) arg1);
}

void func_8010A7EC(s32 arg0, u8 *arg1) {
    func_8000A5F8(3, arg0, arg1[0xC], *(s32 *) (arg1 + 0x10), (s32) arg1);
}
