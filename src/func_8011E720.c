#include "common.h"

extern f32 D_80043DB4;

extern void func_8001CEF8(s32, f32, f32, f32, f32);

void func_8011E720(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    func_8001CEF8(arg0, arg1 * D_80043DB4 / 180.0f, arg2, arg3, arg4);
}
