#include "common.h"

extern void *D_8004AC38;
extern s32 D_8004AC3C;
extern u16 D_8004AC40;
extern u8 D_80043394;

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *);

void *func_80108B80(void) {
    void *r;
    if (D_8004AC38 == 0) {
        D_8004AC38 = func_800054A0(D_8004AC40, 8);
    }
    if (D_8004AC38 == 0) {
        func_80025F24(&D_80043394);
        while (1) {}
    }
    r = D_8004AC38;
    D_8004AC38 = *(void **) D_8004AC38;
    D_8004AC3C++;
    return r;
}
