#include "common.h"

s32 func_8011AD94(f32 *arg0, f32 *arg1) {
    f32 dot = arg0[0] * arg1[0] + arg0[1] * arg1[1] + arg0[2] * arg1[2];

    if (dot == 0.0f) {
        return 0;
    }
    if (dot < 0.0f) {
        return -1;
    }
    return 1;
}
