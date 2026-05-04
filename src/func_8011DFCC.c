#include "common.h"

extern void func_8001DF78(void *, f32, f32, s32, f32, f32, f32);
extern void func_8001DDFC(s32 *, f32, f32, f32);
extern void func_8001CCA4(s32 *, f32, f32, f32);
extern void func_8001E028(void *, f32, f32, s32, f32, f32, f32, f32, f32, f32);
extern void func_8001B320(void *, s32);

void func_8011DFCC(s32 arg0, f32 arg1, f32 arg2, s32 arg3, f32 arg4, f32 arg5, f32 arg6) {
    s32 buf[16];
    func_8001DF78(buf, arg1, arg2, arg3 ^ 0, arg4, arg5, arg6);
    func_8001B320(buf, arg0);
}

void func_8011E028(s32 *arg0, f32 arg1, f32 arg2, f32 arg3,
                   f32 arg4, f32 arg5, f32 arg6,
                   f32 arg7, f32 arg8, f32 arg9) {
    func_8001DDFC(arg0, arg4, arg5, arg6);
    *(f32 *) ((u8 *) arg0 + 0x30) = arg1;
    *(f32 *) ((u8 *) arg0 + 0x34) = arg2;
    *(f32 *) ((u8 *) arg0 + 0x38) = arg3;
    func_8001CCA4(arg0, arg7, arg8, arg9);
}

void func_8011E08C(s32 arg0, f32 arg1, f32 arg2, s32 arg3,
                   f32 arg4, f32 arg5, f32 arg6, f32 arg7, f32 arg8, f32 arg9) {
    s32 buf[16];
    func_8001E028(buf, arg1, arg2, arg3 ^ 0, arg4, arg5, arg6, arg7, arg8, arg9);
    func_8001B320(buf, arg0);
}
