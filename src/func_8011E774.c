#include "common.h"

extern f32 D_80043DB8;

extern void func_8001CEF8(void *, f32, f32, f32, f32);
extern void func_8001B320(void *, s32);

void func_8011E774(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 mtx[16];
    func_8001CEF8(mtx, arg1 * D_80043DB8 / 180.0f, arg2, arg3, arg4);
    func_8001B320(mtx, arg0);
}
