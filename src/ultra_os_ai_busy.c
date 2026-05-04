#include "common.h"

s32 func_8013D7F0(void) {
    register u32 stat = *(volatile u32 *) 0xA450000C;

    if (stat & 0x80000000) {
        return 1;
    } else {
        return 0;
    }
}
