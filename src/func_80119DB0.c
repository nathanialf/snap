#include "common.h"

extern s32 D_80040F40;

extern s32 func_80019D60(s32);
extern u64 func_80037EE0(void);

f32 func_80119DB0(void) {
    D_80040F40 = D_80040F40 * 214013 + 2531011;
    return (D_80040F40 >> 16 & 0xFFFF) / (f32) 0x10000;
}

s32 func_80119E14(s32 arg0) {
    return func_80019D60(arg0) * arg0 / 0x10000;
}

u8 func_80119E54(void) {
    return (u8) func_80037EE0();
}
