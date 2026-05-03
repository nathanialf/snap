#include "common.h"

extern void *D_8004A9AC;
extern s32 D_8004A9E0;
extern u8 D_800432C4;

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *);

void *func_801080A0(void) {
    void *r;
    if (D_8004A9AC == 0) {
        D_8004A9AC = func_800054A0(0x24, 4);
    }
    if (D_8004A9AC == 0) {
        func_80025F24(&D_800432C4);
        while (1) {}
    }
    r = D_8004A9AC;
    D_8004A9AC = *(void **) D_8004A9AC;
    D_8004A9E0++;
    return r;
}
