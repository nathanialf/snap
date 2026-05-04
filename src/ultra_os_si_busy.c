#include "common.h"

s32 func_80137890(void) {
    register u32 stat = *(volatile u32 *) 0xA4800018;

    if (stat & (0x1 | 0x2)) {
        return 1;
    } else {
        return 0;
    }
}
