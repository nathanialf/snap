#include "common.h"

extern void *D_8004A990;
extern s32 D_8004A994;

void func_80107EF8(void **arg0) {
    arg0[0] = D_8004A990;
    D_8004A990 = arg0;
    D_8004A994--;
}
