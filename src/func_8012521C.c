#include "common.h"

extern u16 D_80049750;
extern void func_80025040(s32);
extern void func_80004860(void);
extern s32 func_8003D820(void);

s32 func_8012521C(s32 arg0, s32 arg1) {
    do {
        func_80025040(0x10);
        func_80004860();
        if (arg1 != 0) {
            if (func_8003D820() != arg1) {
                return 1;
            }
        }
    } while (arg0 != D_80049750);
    return 0;
}
