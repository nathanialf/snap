#include "common.h"

extern s32 D_8004A954;
extern s32 D_8004A960;
extern s32 D_8004A964;

void func_801079B8(s32 arg0) {
    D_8004A960 |= arg0;
    if (arg0 & 0x20) {
        D_8004A954 = 3;
    }
    if (arg0 & 0x10) {
        D_8004A954 = 2;
    }
    D_8004A964 = 1;
}
