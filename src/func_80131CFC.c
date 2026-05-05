#include "common.h"

extern s32 func_80032094(s32 *);

s32 func_80131CFC(s32 *arg0, s32 *arg1) {
    s32 saved;
    if ((u32) arg0[2] >= (u32) (arg0[4] + arg0[0])) {
        return 0;
    }
    saved = arg0[2];
    *arg1 = func_80032094(arg0);
    arg0[2] = saved;
    return 1;
}
