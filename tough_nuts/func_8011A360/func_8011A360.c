#include "common.h"

extern f32 func_80036090(f32);

f32 func_8011A360(f32 *arg0) {
    f32 mag = func_80036090(arg0[0] * arg0[0] + arg0[1] * arg0[1] + arg0[2] * arg0[2]);

    if (mag != 0.0f) {
        f32 inv = 1.0f / mag;
        arg0[0] *= inv;
        arg0[1] *= inv;
        arg0[2] *= inv;
        return mag;
    }
    return 0.0f;
}
