#include "common.h"

s32 func_8012C338(s32 *arg0, s32 arg1, s32 arg2) {
    s32 *p = (s32 *) arg0[7];
    if (arg1 == 2) {
        p[arg0[5]] = arg2;
        arg0[5]++;
    }
    return 0;
}
