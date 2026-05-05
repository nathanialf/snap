#include "common.h"

extern s32 D_8004979C;
extern u8 D_800497A8;

extern s32 func_80032840(u8 *, s32, s32);

void func_80106878(void) {
    if (D_8004979C == 1) {
        D_8004979C = 2;
        while (func_80032840(&D_800497A8, 0, 0) != -1) {
        }
        func_80032840(&D_800497A8, 0, 1);
        D_8004979C = 0;
    }
}
