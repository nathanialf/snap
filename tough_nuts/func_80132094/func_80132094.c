#include "common.h"

s32 func_80132094(s32 *arg0) {
    u8 *p = (u8 *) arg0[2];
    u8 b = *p++;
    arg0[2] = (s32) p;
    if (b & 0x80) {
        s32 v = b & 0x7F;
        do {
            p = (u8 *) arg0[2];
            b = *p++;
            arg0[2] = (s32) p;
            v = (v << 7) + (b & 0x7F);
        } while (b & 0x80);
        return v;
    }
    return b;
}
