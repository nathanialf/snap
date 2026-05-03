#include "common.h"

extern s32 *D_8004AF10;
extern s32 *func_800054A0(s32, s32);

s32 *func_8010B6F0(void) {
    s32 *p = D_8004AF10;
    s32 *r;
    if (p != 0) {
        D_8004AF10 = (s32 *) p[0];
        r = p;
    } else {
        r = func_800054A0(0xC, 4);
    }
    return r;
}
