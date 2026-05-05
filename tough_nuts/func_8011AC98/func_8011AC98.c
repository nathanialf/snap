#include "common.h"

f32 *func_8011AC98(f32 *arg0, f32 *arg1) {
    f32 k = (arg1[0] * arg0[0] + arg1[1] * arg0[1] + arg1[2] * arg0[2]) * -2.0f;

    arg0[0] += arg1[0] * k;
    arg0[1] += arg1[1] * k;
    arg0[2] += arg1[2] * k;
    return arg0;
}
