#include "common.h"

extern f32 func_80032A20(f32);
extern f32 func_80038DE0(f32);

void func_8011E2B0(f32 *m, f32 angle1, f32 angle2) {
    f32 s1, s2, c1, c2;
    s1 = func_80032A20(angle1);
    c1 = func_80038DE0(angle1);
    s2 = func_80032A20(angle2);
    c2 = func_80038DE0(angle2);

    m[0]  = c2;
    m[2]  = -s2;
    m[14] = 0.0f;
    m[4]  = s2 * s1;
    m[6]  = c2 * s1;
    m[8]  = s2 * c1;
    m[9]  = -s1;
    m[10] = c2 * c1;
    m[5]  = c1;
    m[1]  = 0.0f;
    m[13] = m[14];
    m[12] = m[14];
    m[11] = m[14];
    m[7]  = m[14];
    m[3]  = m[14];
    m[15] = 1.0f;
}
