#include "common.h"

extern void *D_8004AC10;
extern s32 D_8004AC14;

void func_80108960(void **arg0) {
    arg0[0] = D_8004AC10;
    D_8004AC10 = arg0;
    D_8004AC14--;
}
