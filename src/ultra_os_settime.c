#include "common.h"

extern u64 D_800994C0;
#define __osCurrentTime D_800994C0

void func_80138FA0(u64 time) {
    __osCurrentTime = time;
}
