#include "common.h"

extern void func_8001E2B0(s32 *, f32, f32);

void func_8011E3BC(s32 *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    func_8001E2B0(arg0, arg4, arg5);
    *(f32 *) ((u8 *) arg0 + 0x30) = arg1;
    *(f32 *) ((u8 *) arg0 + 0x34) = arg2;
    *(f32 *) ((u8 *) arg0 + 0x38) = arg3;
}
