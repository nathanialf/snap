#include "common.h"

extern s32 D_8004A910;

void func_80107694(s32 arg0) {
    if (arg0 == 1 || arg0 == 2) {
        D_8004A910 = arg0;
    }
}
