#include "common.h"

s32 func_80132240(s32 *arg0) {
    s32 *base = (s32 *) arg0[0x10];
    return *(s32 *) ((u8 *) base + arg0[0xF] * 0x30 + 0x28);
}
