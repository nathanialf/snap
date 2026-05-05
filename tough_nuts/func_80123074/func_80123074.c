#include "common.h"

extern s8 *D_800968C4;
extern u8 *D_800968C8;
extern void *D_800968E4;

extern void func_80032440(void *, s8);

void func_80123074(arg0, arg1)
s32 arg0;
u8 arg1;
{
    if (D_800968C4[arg0] != -1) {
        D_800968C8[arg0] = arg1;
        func_80032440(D_800968E4, D_800968C4[arg0]);
    }
}
