#include "common.h"

f32 *func_8011AA40(f32 *arg0, s32 mask) {
    if (mask & 0x1) {
        arg0[0] = -arg0[0];
    }
    if (mask & 0x2) {
        arg0[1] = -arg0[1];
    }
    if (mask & 0x4) {
        arg0[2] = -arg0[2];
    }
    return arg0;
}
