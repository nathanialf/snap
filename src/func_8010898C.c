#include "common.h"

extern void *D_8004AC18;
extern s32 D_8004AC1C;
extern u8 D_8004334C;

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *);

void *func_8010898C(void) {
    void *r;
    if (D_8004AC18 == 0) {
        D_8004AC18 = func_800054A0(0xA8, 4);
    }
    if (D_8004AC18 == 0) {
        func_80025F24(&D_8004334C);
        while (1) {}
    }
    r = D_8004AC18;
    D_8004AC18 = *(void **) D_8004AC18;
    D_8004AC1C++;
    return r;
}
