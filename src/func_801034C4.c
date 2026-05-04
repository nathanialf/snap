#include "common.h"

extern s32 D_80003478;

extern void func_8000345C(s32, s32, s32);
extern void func_80002C94(s32, s32, void *, s32);

void func_801034C4(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_8000345C(arg0, arg2, arg3);
    func_80002C94(arg2, arg3, &D_80003478, arg1);
}
