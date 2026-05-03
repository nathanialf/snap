#include "common.h"

extern s32 D_80048888;
extern s32 D_80048890;

void func_801029BC(s32 arg0) {
    D_80048888 = arg0;
}

void func_801029C8(s32 arg0) {
    D_80048890 = arg0;
}

void func_801029D4(void) {
    D_80048890 = 0;
}
