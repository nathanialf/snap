#include "common.h"

extern s32 D_80042D40;
extern u8 D_80099400;
extern u8 D_80099408;

extern void func_80037700(u8 *, u8 *, s32);
extern void func_80032630(u8 *, s32, s32);
extern void func_800359E0(void);
extern void func_80032840(u8 *, s32 *, s32);

void func_801359E0(void) {
    D_80042D40 = 1;
    func_80037700(&D_80099408, &D_80099400, 1);
    func_80032630(&D_80099408, 0, 0);
}

void func_80135A30(void) {
    s32 local;
    if (D_80042D40 == 0) {
        func_800359E0();
    }
    func_80032840(&D_80099408, &local, 1);
}

void func_80135A74(void) {
    func_80032630(&D_80099408, 0, 0);
}
