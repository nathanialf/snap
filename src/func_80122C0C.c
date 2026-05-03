#include "common.h"

extern s32 D_800423E0;

void func_80122C0C(u32 arg0) {
    if (arg0 >= 0x7F01) {
        arg0 = 0x7F00;
    }
    D_800423E0 = arg0;
}
