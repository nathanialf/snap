#include "common.h"

s32 func_8013768C(u8 *s) {
    u8 *p = s;
    if (*p != 0) {
        do {
            p++;
        } while (*p != 0);
    }
    return p - s;
}
