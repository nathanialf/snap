#include "common.h"

extern void func_8001E028(void *, f32, f32, s32, f32, f32, f32, f32, f32, f32);
extern void func_8001B320(void *, s32);

void func_8011E08C(s32 arg0, f32 arg1, f32 arg2, s32 arg3,
                   f32 arg4, f32 arg5, f32 arg6, f32 arg7, f32 arg8, f32 arg9) {
    s32 buf[16];
    func_8001E028(buf, arg1, arg2, arg3 ^ 0, arg4, arg5, arg6, arg7, arg8, arg9);
    func_8001B320(buf, arg0);
}
