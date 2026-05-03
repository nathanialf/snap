#include "common.h"

extern void (*D_8004A948)();

void func_80106E24(s32 arg0) {
    D_8004A948(arg0);
    (*(void (**)()) ((u8 *) arg0 + 0x4))();
}
