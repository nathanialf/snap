#include "common.h"

extern s32 D_8004A958;
extern s32 D_8004A95C;
extern s32 D_8004A964;

void func_80107A04(s32 arg0) {
    D_8004A958 = arg0;
    D_8004A964 = 1;
}

void func_80107A1C(s32 arg0) {
    D_8004A95C = arg0;
    D_8004A964 = 1;
}
