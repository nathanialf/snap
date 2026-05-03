#include "common.h"

extern u8 *D_80048744;
extern u8 *D_80048748;

void func_80100DE4(u8 *arg0) {
    u8 *next = *(u8 **) (arg0 + 0x10);
    if (next != 0) {
        *(u8 **) (next + 0xC) = *(u8 **) (arg0 + 0xC);
    } else {
        D_80048744 = *(u8 **) (arg0 + 0xC);
    }
    {
        u8 *prev = *(u8 **) (arg0 + 0xC);
        if (prev != 0) {
            *(u8 **) (prev + 0x10) = *(u8 **) (arg0 + 0x10);
        } else {
            D_80048748 = *(u8 **) (arg0 + 0x10);
        }
    }
}
