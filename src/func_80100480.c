#include "common.h"

extern u8 D_800484E0;
extern u8 D_800484E1;

void func_80100480(void) {
    if (*(volatile s32 *) 0xA4001000 == 0x17D7) {
        D_800484E0 = 1;
    } else {
        D_800484E0 = 0;
    }
}

void func_801004B0(void) {
    if (*(volatile s32 *) 0xA4000000 == -1) {
        D_800484E1 = 1;
    } else {
        D_800484E1 = 0;
    }
}
