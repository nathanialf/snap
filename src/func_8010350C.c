#include "common.h"

extern void func_800034C4(s32, s32, u8 *, s32);

void func_8010350C(s32 arg0, s32 arg1) {
    u8 buf[1024];
    func_800034C4(arg0, arg1, buf, 0x400);
}
