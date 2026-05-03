#include "common.h"

extern u8 *D_80048734;
extern u8 *D_80048738;

void func_80100D04(u8 *arg0) {
    u8 *next = *(u8 **) (arg0 + 0x10);
    if (next != 0) {
        *(u8 **) (next + 0xC) = *(u8 **) (arg0 + 0xC);
    } else {
        D_80048734 = *(u8 **) (arg0 + 0xC);
    }
    {
        u8 *prev = *(u8 **) (arg0 + 0xC);
        if (prev != 0) {
            *(u8 **) (prev + 0x10) = *(u8 **) (arg0 + 0x10);
        } else {
            D_80048738 = *(u8 **) (arg0 + 0x10);
        }
    }
}
