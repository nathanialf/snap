#include "common.h"

extern void *D_8004AC2C;
extern s32 D_8004AC30;
extern u16 D_8004AC34;
extern u8 D_8004337C;

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *);

void *func_80108AD8(void) {
    void *r;
    if (D_8004AC2C == 0) {
        D_8004AC2C = func_800054A0(D_8004AC34, 8);
    }
    if (D_8004AC2C == 0) {
        func_80025F24(&D_8004337C);
        while (1) {}
    }
    r = D_8004AC2C;
    D_8004AC2C = *(void **) D_8004AC2C;
    D_8004AC30++;
    return r;
}
