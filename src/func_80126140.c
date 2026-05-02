#include "common.h"

void func_80126140(s32 *arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4) {
    *((u8 *) arg0 + 0x18) = arg1;
    *((u8 *) arg0 + 0x19) = arg2;
    *((u8 *) arg0 + 0x1A) = arg3;
    *((u8 *) arg0 + 0x1B) = arg4;
}
