#include "common.h"

extern f32 func_80036090(f32);

#define sqrtf func_80036090

void func_80138510(f32 *x, f32 *y, f32 *z) {
    f32 m;
    m = 1.0f / sqrtf((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
    *x *= m;
    *y *= m;
    *z *= m;
}
