#include "common.h"

s32 func_8013CD50(void) {
    register u32 stat = *(volatile u32 *) 0xA410000C;

    if (stat & 0x100) {
        return 1;
    } else {
        return 0;
    }
}
