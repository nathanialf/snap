#include "common.h"

extern s32 D_8002CFE0;
extern void func_8002D1D8();
extern void func_8002DE50(s32 *, s32 *, void *, s32);
extern s32 *func_8001FD74(s32, s32, s32, s32, s32);

void func_8012B330(s32 *arg0, s32 arg1) {
    func_8002DE50(arg0, &D_8002CFE0, (void *) func_8002D1D8, 1);
    arg0[5] = (s32) func_8001FD74(0, 0, arg1, 1, 0x20);
    arg0[9] = 1;
    arg0[12] = 0;
    arg0[7] = 0;
    arg0[10] = 0;
    arg0[11] = 0;
    *(f32 *) ((u8 *) arg0 + 0x20) = 0.0f;
    *(f32 *) ((u8 *) arg0 + 0x18) = 1.0f;
}
