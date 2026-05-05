#include "common.h"

extern f32 D_80043DC4;

extern void func_8001D26C(s32, f32, f32, f32);

void func_8011E8C0(s32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    func_8001D26C(arg0,
                  D_80043DC4 * arg1 / 180.0f,
                  D_80043DC4 * arg2 / 180.0f,
                  D_80043DC4 * arg3 / 180.0f);
}
