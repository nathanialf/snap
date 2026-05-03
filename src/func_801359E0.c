#include "common.h"

extern s32 D_80042D40;
extern u8 D_80099400;
extern u8 D_80099408;
extern void func_80037700(u8 *, u8 *, s32);
extern void func_80032630(u8 *, s32, s32);

void func_801359E0(void) {
    D_80042D40 = 1;
    func_80037700(&D_80099408, &D_80099400, 1);
    func_80032630(&D_80099408, 0, 0);
}
