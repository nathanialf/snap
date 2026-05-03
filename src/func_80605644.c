#include "common.h"

extern u8 D_803B4DA0;
extern s32 D_8039367C;
extern s32 D_803936B4;
extern s32 D_80393694;
extern s32 D_80393698;

void func_80605644(void) {
    D_803B4DA0 = 0;
    D_8039367C = 0;
}

void func_80605658(s32 arg0) {
    D_803936B4 = arg0;
}

void func_80605664(void) {
    D_80393694 = 0;
}

void func_80605670(void) {
    D_80393694 = 1;
}

s32 func_80605680(void) {
    return D_80393698;
}
