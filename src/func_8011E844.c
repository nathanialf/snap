#include "common.h"

extern f32 D_80043DC0;

extern void func_8001D0C4(void *, f32, f32, f32, f32, f32, f32, f32);
extern void func_8001B320(void *, s32);

void func_8011E844(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7) {
    f32 mtx[16];
    func_8001D0C4(mtx, arg1, arg2, arg3, arg4 * D_80043DC0 / 180.0f, arg5, arg6, arg7);
    func_8001B320(mtx, arg0);
}
