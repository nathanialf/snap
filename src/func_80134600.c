#include "common.h"

extern s32 D_80042CD0;
extern u8 D_80097E90;
extern u8 D_80097E98;

extern void func_80037700(u8 *, u8 *, s32);
extern void func_80032630(u8 *, s32, s32);
extern void func_80034600(void);
extern void func_80032840(u8 *, s32 *, s32);

void func_80134600(void) {
    D_80042CD0 = 1;
    func_80037700(&D_80097E98, &D_80097E90, 1);
    func_80032630(&D_80097E98, 0, 0);
}

void func_80134650(void) {
    s32 local;
    if (D_80042CD0 == 0) {
        func_80034600();
    }
    func_80032840(&D_80097E98, &local, 1);
}

void func_80134694(void) {
    func_80032630(&D_80097E98, 0, 0);
}
