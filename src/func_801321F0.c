#include "common.h"

extern s32 func_80032980(s32, s32 *);

void func_801321F0(s32 **arg0, s32 *arg1) {
    s32 r = func_80032980(1, arg1);
    arg1[4] = (s32) arg0[8];
    arg1[0] = (s32) arg0[0];
    arg0[0] = arg1;
    func_80032980(r, arg1);
}
