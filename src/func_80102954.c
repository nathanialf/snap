#include "common.h"

extern s32 D_80048888;
extern s32 D_8004888C;
extern s32 D_80048890;
extern u8 D_800488A4;
extern u8 D_800488A8;

extern void func_8003D240(f32);
extern void func_80035660(s32);
extern void func_80004A04(s32);
extern void func_800049E4(s32);
extern void func_80037700(u8 *, u8 *, s32);

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

void func_801029BC(s32 arg0) {
    D_80048888 = arg0;
}

void func_801029C8(s32 arg0) {
    D_80048890 = arg0;
}

void func_801029D4(void) {
    D_80048890 = 0;
}

void func_801029E0(void) {
    func_80037700(&D_800488A8, &D_800488A4, 1);
}
