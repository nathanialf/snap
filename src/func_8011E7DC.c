#include "common.h"

extern f32 D_80043DBC;

extern void func_8001D0C4(s32, f32, f32, f32, f32, f32, f32, f32);

void func_8011E7DC(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7) {
    func_8001D0C4(arg0, arg1, arg2, arg3, arg4 * D_80043DBC / 180.0f, arg5, arg6, arg7);
}
