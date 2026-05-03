#include "common.h"

extern void *D_8004AAE8;
extern s32 D_8004ABFC;

void func_80108460(void **arg0) {
    arg0[1] = D_8004AAE8;
    D_8004AAE8 = arg0;
    D_8004ABFC--;
}
