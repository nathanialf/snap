#include "common.h"

extern s32 D_8004A964;
extern s32 D_80048868;

extern void func_80007A94(s32 *);
extern void func_80032630(s32 *, s32 *, s32);

void func_80107AFC(s32 *arg0) {
    if (D_8004A964 != 0) {
        arg0[0] = 4;
        arg0[1] = 0x32;
        arg0[5] = 0;
        arg0[8] = 0;
        func_80007A94(arg0);
        func_80032630(&D_80048868, arg0, 0);
    }
}
