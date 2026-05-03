#include "common.h"

extern s32 func_80026170(s32 *, s32);
extern s32 func_800287F0(s32 *, s32);

void func_80128850(s32 *arg0, f32 arg1, f32 arg2) {
    *(f32 *) ((u8 *) arg0 + 8)  = arg1;
    *(f32 *) ((u8 *) arg0 + 12) = arg2;
    if (arg1 == 1.0f && arg2 == 1.0f) {
        func_800287F0(arg0, 0x10);
    } else {
        func_80026170(arg0, 0x10);
    }
}
