#include "common.h"

extern s32 D_800423B8;
extern u8 D_800484E8;
extern void func_80032840(u8 *, s32, s32);

void func_8012311C(void) {
    func_80032840(&D_800484E8, 0, 0);
    D_800423B8 = 1;
    func_80032840(&D_800484E8, 0, 1);
}
