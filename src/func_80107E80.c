#include "common.h"

extern void *D_8004A990;
extern s32 D_8004A994;
extern u8 D_80043280;

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *);

void *func_80107E80(void) {
    void *r;
    if (D_8004A990 == 0) {
        D_8004A990 = func_800054A0(0x1C0, 8);
    }
    if (D_8004A990 == 0) {
        func_80025F24(&D_80043280);
        while (1) {}
    }
    r = D_8004A990;
    D_8004A990 = *(void **) D_8004A990;
    D_8004A994++;
    return r;
}
