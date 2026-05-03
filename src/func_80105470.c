#include "common.h"

extern s32 D_8004A8C8;
extern s32 func_800078C4(s32 *, s32, s32, s32);

void func_80105470(s32 arg0, s32 arg1) {
    func_800078C4(&D_8004A8C8, 0x10000, arg0, arg1);
}
