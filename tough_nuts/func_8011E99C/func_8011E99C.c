#include "common.h"

extern f32 D_80043DCC;

extern void func_8001D648(s32, f32, f32, f32, f32, f32, f32);

void func_8011E99C(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6) {
    func_8001D648(arg0, arg1, arg2, arg3,
                  arg4 * D_80043DCC / 180.0f,
                  arg5 * D_80043DCC / 180.0f,
                  arg6 * D_80043DCC / 180.0f);
}
