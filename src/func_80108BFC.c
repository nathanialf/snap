#include "common.h"

extern void *D_8004AC38;
extern s32 D_8004AC3C;

void func_80108BFC(void **arg0) {
    arg0[0] = D_8004AC38;
    D_8004AC38 = arg0;
    D_8004AC3C--;
}
