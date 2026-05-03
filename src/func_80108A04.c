#include "common.h"

extern void *D_8004AC18;
extern s32 D_8004AC1C;

void func_80108A04(void **arg0) {
    arg0[0] = D_8004AC18;
    D_8004AC18 = arg0;
    D_8004AC1C--;
}
