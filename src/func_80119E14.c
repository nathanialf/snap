#include "common.h"

extern s32 func_80019D60(s32);

s32 func_80119E14(s32 arg0) {
    return func_80019D60(arg0) * arg0 / 0x10000;
}
