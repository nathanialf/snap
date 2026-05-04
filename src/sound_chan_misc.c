#include "common.h"

extern s8 *D_800968C4;
extern s32 *D_800968BC;
extern s32 *D_800968C0;
extern void *D_800968E4;

extern s32 func_80032980(s32);
extern void func_8002A500(void *, s32);
extern s32 func_80032240(void *);
extern void func_8002A640(void *, f32);
extern void func_800289C0(void *);

void func_80122EEC(s32 arg0, f32 arg1) {
    s32 saved;

    if (D_800968C4[arg0] == -1) return;
    saved = func_80032980(1);
    func_8002A500(D_800968E4, D_800968C4[arg0]);
    if (func_80032240(D_800968E4) == 1 || D_800968C0[arg0] >= 0) {
        if (arg1 > 2.0) arg1 = 2.0f;
        func_8002A640(D_800968E4, arg1);
    }
    func_80032980(saved);
}

void func_80122FD0(s32 arg0) {
    s32 saved;

    if (D_800968C4[arg0] != -1) {
        saved = func_80032980(1);
        func_8002A500(D_800968E4, D_800968C4[arg0]);
        func_800289C0(D_800968E4);
        func_80032980(saved);
    }
    D_800968BC[arg0] = -1;
    D_800968C0[arg0] = -1;
}

