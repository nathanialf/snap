#include "common.h"

extern s32 D_80042D10;
extern s32 D_80042D18;

s32 func_80138DB0(void) {
    if (D_80042D10 == 0) {
        return 0;
    }
    return D_80042D18;
}
