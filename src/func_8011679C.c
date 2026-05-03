#include "common.h"

extern f32 D_8004B030;
extern f32 D_8004B02C;
extern f32 D_8004AF24;
extern s32 D_8004B034;
extern void func_80016548(s32);

void func_8011679C(s32 *arg0) {
    D_8004AF24 = D_8004B02C = D_8004B030 = 1.0f;
    D_8004B034 = 0;
    func_80016548(arg0[0x12]);
}
