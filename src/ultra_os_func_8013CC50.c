#include "common.h"

extern s32 func_80037890(void);

s32 func_8013CC50(u32 devAddr, u32 data) {
    if (func_80037890() != 0) {
        return -1;
    }
    *(u32 *) (devAddr | 0xA0000000) = data;
    return 0;
}
