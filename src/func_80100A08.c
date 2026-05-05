#include "common.h"

extern void func_80037700(s32, s32, s32);
extern void func_800009A0(s32 *);

void func_80100A08(s32 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 buf[10];

    func_80037700(arg1, arg2, arg3);
    arg0[1] = arg1;
    buf[0] = 3;
    buf[1] = 0x64;
    buf[9] = (s32) arg0;
    func_800009A0(buf);
}
