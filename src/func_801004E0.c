#include "common.h"

extern u8 D_80042FA0[];
extern void func_80025F24(u8 *, s32);

void func_801004E0(s32 arg0) {
    func_80025F24(D_80042FA0, arg0);
    while (1) {}
}
