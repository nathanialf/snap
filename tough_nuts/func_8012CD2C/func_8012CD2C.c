#include "common.h"

f64 func_8012CD2C(f64 x, s32 n) {
    if (n != 0) {
        s32 shift = 1 << n;
        f64 m = (f64) shift;
        x *= m;
    }
    return x;
}
