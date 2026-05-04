#include "common.h"

s32 func_80137860(void) {
    register u32 stat = *(volatile u32 *) 0xA4040010;

    if (stat & (0x4 | 0x8 | 0x10)) {
        return 1;
    } else {
        return 0;
    }
}
