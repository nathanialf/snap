#include "common.h"

extern f32 func_8001A360(f32 *);

f32 *func_8011A43C(f32 *arg0, f32 *arg1) {
    arg0[0] += arg1[0];
    arg0[1] += arg1[1];
    arg0[2] += arg1[2];
    return arg0;
}

f32 *func_8011A474(f32 *arg0, f32 *arg1) {
    arg0[0] -= arg1[0];
    arg0[1] -= arg1[1];
    arg0[2] -= arg1[2];
    return arg0;
}

f32 *func_8011A4AC(f32 *arg0, f32 *arg1, f32 *arg2) {
    arg0[0] = arg1[0] - arg2[0];
    arg0[1] = arg1[1] - arg2[1];
    arg0[2] = arg1[2] - arg2[2];
    return arg0;
}

f32 func_8011A4E4(f32 *arg0, f32 *arg1, f32 *arg2) {
    arg0[0] = arg1[0] - arg2[0];
    arg0[1] = arg1[1] - arg2[1];
    arg0[2] = arg1[2] - arg2[2];
    return func_8001A360(arg0);
}
