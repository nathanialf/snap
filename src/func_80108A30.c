#include "common.h"

extern void *D_8004AC20;
extern s32 D_8004AC24;
extern u16 D_8004AC28;
extern u8 D_80043364;

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *);

void *func_80108A30(void) {
    void *r;
    if (D_8004AC20 == 0) {
        D_8004AC20 = func_800054A0(D_8004AC28, 8);
    }
    if (D_8004AC20 == 0) {
        func_80025F24(&D_80043364);
        while (1) {}
    }
    r = D_8004AC20;
    D_8004AC20 = *(void **) D_8004AC20;
    D_8004AC24++;
    return r;
}
