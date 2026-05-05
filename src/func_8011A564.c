#include "common.h"

f32 *func_8011A564(f32 *arg0, f32 arg1) {
    if (arg1 != 0.0f) {
        arg0[0] /= arg1;
        arg0[1] /= arg1;
        arg0[2] /= arg1;
    }
    return arg0;
}
