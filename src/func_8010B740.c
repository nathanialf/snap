#include "common.h"

extern s32 *D_8004AF10;

void func_8010B740(s32 **arg0) {
    s32 *p = arg0[1];
    if (p != 0) {
        p[0] = (s32) D_8004AF10;
        D_8004AF10 = arg0[0];
        arg0[0] = 0;
        arg0[1] = 0;
    }
}
