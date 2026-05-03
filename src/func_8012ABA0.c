#include "common.h"

extern s32 *D_800428F0;

void func_8012ABA0(s32 *arg0) {
    s32 *p = D_800428F0;
    arg0[0] = p[0xB];
    p[0xB] = (s32) arg0;
}
