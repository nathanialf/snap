#include "common.h"

extern void *D_8004AC2C;
extern s32 D_8004AC30;

void func_80108B54(void **arg0) {
    arg0[0] = D_8004AC2C;
    D_8004AC2C = arg0;
    D_8004AC30--;
}
