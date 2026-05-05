#include "common.h"

extern s32 D_8004888C;

extern void func_8003D240(f32);
extern void func_80035660(s32);
extern void func_80004A04(s32);
extern void func_800049E4(s32);

void func_80102954(void) {
    s32 i;

    D_8004888C = 1;
    func_8003D240(1.0f);
    func_80035660(1);
    for (i = 0; i < 4; i++) {
        func_80004A04(i);
        func_800049E4(i);
    }
}
