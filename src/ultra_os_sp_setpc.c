#include "common.h"

s32 func_8013D860(u32 pc) {
    register u32 stat = *(volatile u32 *) 0xA4040010;

    if (!(stat & 0x1)) {
        return -1;
    }
    *(volatile u32 *) 0xA4080000 = pc;

    return 0;
}
