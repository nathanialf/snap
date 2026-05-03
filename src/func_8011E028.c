#include "common.h"

extern void func_8001DDFC(s32 *, f32, f32, f32);
extern void func_8001CCA4(s32 *, f32, f32, f32);

void func_8011E028(s32 *arg0, f32 arg1, f32 arg2, f32 arg3,
                   f32 arg4, f32 arg5, f32 arg6,
                   f32 arg7, f32 arg8, f32 arg9) {
    func_8001DDFC(arg0, arg4, arg5, arg6);
    *(f32 *) ((u8 *) arg0 + 0x30) = arg1;
    *(f32 *) ((u8 *) arg0 + 0x34) = arg2;
    *(f32 *) ((u8 *) arg0 + 0x38) = arg3;
    func_8001CCA4(arg0, arg7, arg8, arg9);
}
