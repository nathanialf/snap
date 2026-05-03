#include "common.h"

extern s32 D_8004A958;
extern s32 D_8004A95C;

void func_80107CBC(s16 *arg0) {
    s16 v;
    v = D_8004A958 * 2;
    arg0[4] = v;
    arg0[0] = v;
    v = D_8004A95C * 2;
    arg0[6] = 0x1FF;
    arg0[5] = v;
    arg0[1] = v;
    arg0[2] = arg0[6];
}
