#include "common.h"

s32 func_80119F00(u8 *dst, u8 *src, s32 count) {
    s32 ret = count;
    if (count != 0) {
        count--;
        do {
            u8 a = *src;
            u8 b = *dst;
            ret = count;
            *dst = a;
            dst++;
            src++;
            *(src - 1) = b;
        } while (count-- != 0);
    }
    return ret;
}
