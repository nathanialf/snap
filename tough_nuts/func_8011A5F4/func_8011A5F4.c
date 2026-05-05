#include "common.h"

extern f32 func_8001A360(f32 *);

f32 func_8011A5F4(f32 *arg0, f32 arg1, f32 *arg2, f32 arg3) {
    arg0[0] *= arg1;
    arg0[1] *= arg1;
    arg0[2] *= arg1;
    arg0[0] += arg2[0] * arg3;
    arg0[1] += arg2[1] * arg3;
    arg0[2] += arg2[2] * arg3;
    return func_8001A360(arg0);
}
