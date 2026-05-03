#include "common.h"

extern s32 D_80048888;
extern s32 D_80048890;
extern u8 D_800488A4;
extern u8 D_800488A8;
extern void func_80037700(u8 *, u8 *, s32);

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
