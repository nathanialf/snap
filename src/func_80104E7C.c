#include "common.h"

extern s8 D_80048A03;

s32 func_80104E7C(void) {
    if (D_80048A03 == 5) {
        return 1;
    }
    return 0;
}
