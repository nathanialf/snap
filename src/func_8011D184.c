#include "common.h"

extern void func_8001CEF8(s32 *, s32, s32, s32, f32);
extern void func_8001CCA4(s32 *, s32, s32, s32);

void func_8011D184(s32 *arg0, f32 arg1, f32 arg2, f32 arg3,
                   s32 arg4, s32 arg5, s32 arg6, f32 arg7,
                   s32 arg8, s32 arg9, s32 arg10) {
    func_8001CEF8(arg0, arg4, arg5, arg6, arg7);
    *(f32 *) ((u8 *) arg0 + 0x30) = arg1;
    *(f32 *) ((u8 *) arg0 + 0x34) = arg2;
    *(f32 *) ((u8 *) arg0 + 0x38) = arg3;
    func_8001CCA4(arg0, arg8, arg9, arg10);
}
