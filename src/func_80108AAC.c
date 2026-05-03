#include "common.h"

extern void *D_8004AC20;
extern s32 D_8004AC24;

void func_80108AAC(void **arg0) {
    arg0[0] = D_8004AC20;
    D_8004AC20 = arg0;
    D_8004AC24--;
}
