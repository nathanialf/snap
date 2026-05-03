#include "common.h"

extern f32 func_8001A3FC(f32 *);

f32 func_8011AF30(f32 *arg0, f32 *arg1) {
    f32 v[3];
    v[0] = arg1[0] - arg0[0];
    v[1] = arg1[1] - arg0[1];
    v[2] = arg1[2] - arg0[2];
    return func_8001A3FC(v);
}
