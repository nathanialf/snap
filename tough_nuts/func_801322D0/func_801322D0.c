#include "common.h"

extern f32 D_80044880;
extern f32 D_80044884;

f32 func_801322D0(s32 n) {
    f32 base;
    f32 result = 1.0f;
    if (n < 0) {
        n = -n;
        base = D_80044884;
    } else {
        base = D_80044880;
    }
    while (n != 0) {
        if (n & 1) {
            result *= base;
        }
        n >>= 1;
        base *= base;
    }
    return result;
}
