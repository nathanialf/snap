#include "common.h"

extern void func_8001CEF8(s32 *, f32, f32, f32, f32);

void func_8011D0C4(s32 *arg0, f32 arg1, f32 arg2, f32 arg3,
                   f32 arg4, f32 arg5, f32 arg6, f32 arg7) {
    func_8001CEF8(arg0, arg4, arg5, arg6, arg7);
    *(f32 *) ((u8 *) arg0 + 0x30) = arg1;
    *(f32 *) ((u8 *) arg0 + 0x34) = arg2;
    *(f32 *) ((u8 *) arg0 + 0x38) = arg3;
}
