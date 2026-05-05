#include "common.h"

extern f64 D_800442B0;

s32 func_8012AC50(s32 *arg0, s32 arg1) {
    f32 prod = (f32) arg1 * (f32) arg0[17];

    return (s32) (f32) ((f64) prod / D_800442B0 + 0.5);
}
