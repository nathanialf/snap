#include "common.h"

extern s16 D_8004AC02;

void func_8010AD88(s32 arg0) {
    D_8004AC02 = arg0;
}

s32 func_8010AD94(void) {
    return D_8004AC02;
}
