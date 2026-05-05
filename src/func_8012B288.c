#include "common.h"

extern s32 D_8002D2D0;
extern void func_8002DAC0();
extern void func_8002DE50(s32 *, s32 *, void *, s32);
extern s32 *func_8001FD74(s32, s32, s32, s32, s32);

void func_8012B288(s32 *arg0, s32 (*callback)(s32 *), s32 arg2) {
    func_8002DE50(arg0, &D_8002D2D0, (void *) func_8002DAC0, 0);
    arg0[5] = (s32) func_8001FD74(0, 0, arg2, 1, 0x20);
    arg0[6] = (s32) func_8001FD74(0, 0, arg2, 1, 0x20);
    arg0[12] = callback((s32 *) ((u8 *) arg0 + 0x34));
    arg0[15] = 0;
    arg0[16] = 1;
    arg0[17] = 0;
}
