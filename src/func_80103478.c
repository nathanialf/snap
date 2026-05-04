#include "common.h"

extern s32 D_800488C0;
extern s32 D_800488C4;
extern s32 D_800488C8;

extern void func_80002C20(s32, s32, s32);

void func_80103478(void) {
    func_80002C20(D_800488C8, D_800488C0, D_800488C4);
    D_800488C8 += D_800488C4;
}
