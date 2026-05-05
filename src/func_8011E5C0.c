#include "common.h"

extern f32 func_80032A20(f32);
extern f32 func_80038DE0(f32);

void func_8011E5C0(f32 *m, f32 angle) {
    f32 s = func_80032A20(angle);
    f32 c = func_80038DE0(angle);

    m[0]  = c;
    m[2]  = -s;
    m[15] = 1.0f;
    m[10] = c;
    m[8]  = s;
    m[9]  = 0.0f;
    m[4]  = 0.0f;
    m[6]  = 0.0f;
    m[1]  = 0.0f;
    m[14] = 0.0f;
    m[13] = 0.0f;
    m[12] = 0.0f;
    m[11] = 0.0f;
    m[7]  = 0.0f;
    m[3]  = 0.0f;
    m[5]  = *(volatile f32 *) &m[15];
}
