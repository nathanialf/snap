#include "common.h"

extern void *D_8004AC44;
extern s32 D_8004AC54;

extern void func_8000A52C(void *);

s32 *func_8010AE4C(s32 *arg0) {
    s32 *ret;
    D_8004AC44 = arg0;
    ((void (*)(void)) arg0[5])();
    ret = (s32 *) arg0[1];
    D_8004AC44 = 0;
    switch (D_8004AC54) {
        case 0:
            break;
        case 2:
            D_8004AC54 = 0;
            func_8000A52C(arg0);
            break;
        default:
            D_8004AC54 = 0;
            break;
    }
    return ret;
}
