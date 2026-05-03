#include "common.h"

extern u8 *D_80048750;
extern u8 *D_8004874C;

void func_80100E34(u8 *arg0) {
    *(u8 **) (arg0 + 0xC) = 0;
    *(u8 **) (arg0 + 0x10) = D_80048750;
    if (D_80048750 != 0) {
        *(u8 **) (D_80048750 + 0xC) = arg0;
    } else {
        D_8004874C = arg0;
    }
    D_80048750 = arg0;
}

void func_80100E6C(u8 *arg0) {
    u8 *next = *(u8 **) (arg0 + 0x10);
    if (next != 0) {
        *(u8 **) (next + 0xC) = *(u8 **) (arg0 + 0xC);
    } else {
        D_8004874C = *(u8 **) (arg0 + 0xC);
    }
    {
        u8 *prev = *(u8 **) (arg0 + 0xC);
        if (prev != 0) {
            *(u8 **) (prev + 0x10) = *(u8 **) (arg0 + 0x10);
        } else {
            D_80048750 = *(u8 **) (arg0 + 0x10);
        }
    }
}
