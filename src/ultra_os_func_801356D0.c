#include "common.h"

extern s32 func_80037890(void);

s32 func_801356D0(u32 devAddr, u32 *outData) {
    if (func_80037890() != 0) {
        return -1;
    }
    *outData = *(u32 *) (devAddr | 0xA0000000);
    return 0;
}
