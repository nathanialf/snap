#include "common.h"

extern void *D_8004AC10;
extern s32 D_8004AC14;
extern u8 D_80043334;

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *);

void *func_801088B8(void) {
    void *r;
    if (D_8004AC10 == 0) {
        D_8004AC10 = func_800054A0(0x24, 4);
    }
    if (D_8004AC10 == 0) {
        func_80025F24(&D_80043334);
        while (1) {}
    }
    r = D_8004AC10;
    D_8004AC10 = *(void **) D_8004AC10;
    D_8004AC14++;
    return r;
}
