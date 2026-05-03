#include "common.h"

extern void *D_8004AC04;
extern s32 D_8004AC08;
extern u8 D_80043318;

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *);

void *func_80108814(void) {
    void *r;
    if (D_8004AC04 == 0) {
        D_8004AC04 = func_800054A0(0x48, 8);
    }
    if (D_8004AC04 == 0) {
        func_80025F24(&D_80043318);
        while (1) {}
    }
    r = D_8004AC04;
    D_8004AC04 = *(void **) D_8004AC04;
    D_8004AC08++;
    return r;
}
