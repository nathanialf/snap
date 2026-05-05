#include "common.h"

extern f32 func_80036090(f32);

f32 func_8011A3FC(f32 *arg0) {
    return func_80036090(arg0[0] * arg0[0] + arg0[1] * arg0[1] + arg0[2] * arg0[2]);
}
