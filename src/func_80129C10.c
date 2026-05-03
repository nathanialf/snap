#include "common.h"

extern s32 *D_800428F0;
extern void func_8002A6A0(s32);
extern void func_800325A0(void);

void func_80129C10(s32 arg0) {
    if (D_800428F0 == 0) {
        D_800428F0 = (s32 *) arg0;
        func_8002A6A0(arg0);
    }
}

void func_80129C44(void) {
    if (D_800428F0 != 0) {
        func_800325A0();
        D_800428F0 = 0;
    }
}
