#include "common.h"

extern u8 *D_8004AC44;

void func_8010BD78(u8 *arg0) {
    u8 *p;
    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    p = *(u8 **) (arg0 + 0x18);
    while (p != 0) {
        p[0x15] = 0;
        p = *(u8 **) p;
    }
}
