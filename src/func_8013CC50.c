#include "common.h"

extern s32 func_80037890(u32);

s32 func_8013CC50(u32 arg0, u32 arg1) {
    if (func_80037890(arg0) != 0) {
        return -1;
    }
    *(volatile u32 *) (arg0 | 0xA0000000) = arg1;
    return 0;
}
