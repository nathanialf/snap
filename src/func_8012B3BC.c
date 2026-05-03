#include "common.h"

extern void func_8002DE50(s32 *, s32 *, void (*)(void), s32);
extern s32 D_8002C260;
extern void func_8002C338(void);
extern s32 D_8002B4B0;
extern void func_8002B5D0(void);
extern s32 D_8002DFB0;
extern void func_8002E03C(void);

void func_8012B3BC(s32 *arg0, s32 arg1, s32 arg2) {
    func_8002DE50(arg0, &D_8002C260, func_8002C338, 6);
    arg0[5] = 0;
    arg0[6] = arg2;
    arg0[7] = arg1;
}

void func_8012B410(s32 *arg0, s32 arg1, s32 arg2) {
    func_8002DE50(arg0, &D_8002B4B0, func_8002B5D0, 7);
    arg0[5] = 0;
    arg0[6] = arg2;
    arg0[7] = arg1;
}

void func_8012B464(s32 *arg0) {
    func_8002DE50(arg0, &D_8002DFB0, func_8002E03C, 3);
    arg0[5] = 0;
    arg0[6] = 1;
}
