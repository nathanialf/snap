#include "common.h"

f32 *func_8011A5AC(f32 *arg0, f32 *arg1, f32 arg2) {
    arg0[0] += arg1[0] * arg2;
    arg0[1] += arg1[1] * arg2;
    arg0[2] += arg1[2] * arg2;
    return arg0;
}
