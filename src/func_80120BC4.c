#include "common.h"

extern s32 D_80096270;
extern s32 D_80096278;
extern s32 D_800962C0;
extern s32 D_800962A8;
extern s32 D_800962C8;
extern s32 D_80096290;
extern s32 D_800962C4;

extern void func_80000A08(s32 *, s32 *, s32 *, s32);
extern void func_80037700(s32 *, s32 *, s32);
extern void func_80032630(s32 *, s32, s32);

void func_80120BC4(void) {
    func_80000A08(&D_80096270, &D_80096278, &D_800962C0, 1);
    func_80037700(&D_800962A8, &D_800962C8, 0x32);
    func_80037700(&D_80096290, &D_800962C4, 1);
    func_80032630(&D_80096290, 0, 1);
}
