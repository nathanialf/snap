#include "common.h"

extern f32 D_800426C0[];

extern void func_80024054(s32, s32, s32, s32, s32, s32);

void func_80124228(s32 arg0, s32 arg1, f32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    func_80024054(arg0, arg1, (s32) (D_800426C0[arg4] * arg2), arg3, arg4, arg5);
}
