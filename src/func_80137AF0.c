#include "common.h"

extern s32 *D_80042CC0;

s32 func_80137AF0(s32 *arg0) {
    if (arg0 == 0) {
        arg0 = D_80042CC0;
    }
    return arg0[1];
}
