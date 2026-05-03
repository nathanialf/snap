#include "common.h"

extern void *D_80096970;

void func_80120BAC(void **arg0) {
    *arg0 = D_80096970;
    D_80096970 = arg0;
}
