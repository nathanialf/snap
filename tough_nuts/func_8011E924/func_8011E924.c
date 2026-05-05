#include "common.h"

extern f32 D_80043DC8;

extern void func_8001D26C(void *, f32, f32, f32);
extern void func_8001B320(void *, s32);

void func_8011E924(s32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    f32 mtx[16];
    func_8001D26C(mtx,
                  arg1 * D_80043DC8 / 180.0f,
                  arg2 * D_80043DC8 / 180.0f,
                  arg3 * D_80043DC8 / 180.0f);
    func_8001B320(mtx, arg0);
}
