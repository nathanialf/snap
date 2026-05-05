#include "common.h"

f32 func_80130018(f32 arg0, s32 arg1, f32 arg2) {
    f32 result;
    s32 i;
    s32 bit;

    arg1 >>= 3;
    if (arg1 == 0) {
        return arg0;
    }
    result = 1.0f;
    i = 0;
    bit = arg1 & 1;
    do {
        if (bit != 0) {
            result *= arg2;
        }
        arg1 >>= 1;
        if (arg1 == 0) {
            break;
        }
        arg2 *= arg2;
        i++;
        if (i == 32) {
            break;
        }
        bit = arg1 & 1;
    } while (1);
    return arg0 * result;
}
