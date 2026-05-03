#include "common.h"

extern s32 D_80000A64;
extern s32 *D_8004A94C;

void func_801053F0(s32 *arg0) {
    if (arg0 != 0) {
        D_8004A94C = arg0;
    } else {
        D_8004A94C = &D_80000A64;
    }
}
