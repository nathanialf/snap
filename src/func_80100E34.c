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
