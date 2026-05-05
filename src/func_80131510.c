#include "common.h"

typedef struct A {
    u8 _0[0x20];
    u8 *_20;
    u8 _24[0xD];
    u8 _31;
} A;

typedef struct B {
    u8 _0[0x64];
    u8 *_64;
} B;

u8 func_80131510(A *arg0, B *arg1) {
    s32 v = arg0->_20[0xC] + arg1->_64[arg0->_31 * 20 + 7] - 0x40;
    if (v <= 0) {
        v = 0;
    }
    if (v >= 0x7F) {
        v = 0x7F;
    }
    return v;
}
