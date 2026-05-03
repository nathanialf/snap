#include "common.h"

extern void *D_8004A9AC;
extern s32 D_8004A9E0;

void func_801081F0(void **arg0) {
    arg0[0] = D_8004A9AC;
    D_8004A9AC = arg0;
    D_8004A9E0--;
}
