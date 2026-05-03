#include "common.h"

extern void func_8001DDFC(s32 *, f32, f32, f32);

void func_8011DF78(s32 *arg0, f32 arg1, f32 arg2, f32 arg3,
                   f32 arg4, f32 arg5, f32 arg6) {
    func_8001DDFC(arg0, arg4, arg5, arg6);
    *(f32 *) ((u8 *) arg0 + 0x30) = arg1;
    *(f32 *) ((u8 *) arg0 + 0x34) = arg2;
    *(f32 *) ((u8 *) arg0 + 0x38) = arg3;
}
