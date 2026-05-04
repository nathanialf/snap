#include "common.h"

extern void func_8001BBC4(void *, s32, f32, f32, f32, f32, f32, f32, f32, f32, f32);
extern void func_8001B0F0(void *, s32);

void func_8011BFAC(s32 arg0, s32 arg1, f32 arg2, f32 arg3,
                   f32 arg4, f32 arg5, f32 arg6, f32 arg7, f32 arg8, f32 arg9, f32 arg10) {
    s32 buf[16];
    func_8001BBC4(buf, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    func_8001B0F0(buf, arg0);
}
