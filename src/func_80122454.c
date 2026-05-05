#include "common.h"

extern void func_8002DE70(s32, s16);
extern f32 D_80096920[];
extern s32 D_80096918[];
extern s32 D_800968F0[];

void func_80122454(s32 arg0, u32 arg1) {
    if (arg1 >= 0x7F01) {
        arg1 = 0x7F00;
    }
    func_8002DE70(D_800968F0[arg0], (s16)arg1);
    D_80096920[arg0] = (f32)arg1;
    D_80096918[arg0] = 0;
}
