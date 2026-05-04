#include "common.h"

extern void func_8001CEF8(s32 *, f32, f32, f32, f32);
extern void func_8001D0C4(void *, f32, f32, s32, f32, f32, f32, f32);
extern void func_8001B320(void *, s32);

void func_8011D0C4(s32 *arg0, f32 arg1, f32 arg2, f32 arg3,
                   f32 arg4, f32 arg5, f32 arg6, f32 arg7) {
    func_8001CEF8(arg0, arg4, arg5, arg6, arg7);
    *(f32 *) ((u8 *) arg0 + 0x30) = arg1;
    *(f32 *) ((u8 *) arg0 + 0x34) = arg2;
    *(f32 *) ((u8 *) arg0 + 0x38) = arg3;
}

void func_8011D120(s32 arg0, f32 arg1, f32 arg2, s32 arg3,
                   f32 arg4, f32 arg5, f32 arg6, f32 arg7) {
    s32 buf[16];
    func_8001D0C4(buf, arg1, arg2, arg3 ^ 0, arg4, arg5, arg6, arg7);
    func_8001B320(buf, arg0);
}
