#include "common.h"

extern s8 *D_800968C4;
extern s32 *D_800968C0;
extern void *D_800968E4;

extern s32 func_80032980(s32);
extern void func_8002A500(void *, s32);
extern s32 func_80032240(void *);
extern void func_80032190(void *, s32);
extern void func_8002A460(void *, s32);

void func_80122D54(arg0, arg1)
s32 arg0;
s32 arg1;
{
    s32 saved;

    if (D_800968C4[arg0] == -1) return;
    saved = func_80032980(1);
    func_8002A500(D_800968E4, D_800968C4[arg0]);
    if (func_80032240(D_800968E4) == 1 || D_800968C0[arg0] >= 0) {
        if (!((u32)arg1 < 0x80U)) arg1 = 0x7F;
        func_80032190(D_800968E4, ((u8 *)&arg1)[3]);
    }
    func_80032980(saved);
}

void func_80122E20(arg0, arg1)
s32 arg0;
s32 arg1;
{
    s32 saved;

    if (D_800968C4[arg0] == -1) return;
    saved = func_80032980(1);
    func_8002A500(D_800968E4, D_800968C4[arg0]);
    if (func_80032240(D_800968E4) == 1 || D_800968C0[arg0] >= 0) {
        if (!((u32)arg1 < 0x80U)) arg1 = 0x7F;
        func_8002A460(D_800968E4, ((u8 *)&arg1)[3]);
    }
    func_80032980(saved);
}
