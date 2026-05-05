#include "common.h"

typedef struct {
    s32 _0;
    s32 _4;
    s32 _8;
    s32 _C;
    s32 _10;
    s32 _14;
    s32 _18;
    s32 _1C;
    s32 _20;
} Foo;

extern u8 *D_800488A0;

extern void func_80037730(s32, s32);
extern void func_800377B0(s32, s32);
extern void func_80002A10(u8 *, s32, s32, s32, s32);
extern void func_80033BE0(s32, s32);

void func_80102B64(Foo *arg0) {
    if (arg0->_10 - arg0->_C != 0) {
        func_80037730(arg0->_C, arg0->_10 - arg0->_C);
        func_800377B0(arg0->_C, arg0->_10 - arg0->_C);
    }
    if (arg0->_18 - arg0->_14 != 0) {
        func_800377B0(arg0->_14, arg0->_18 - arg0->_14);
    }
    if (arg0->_4 - arg0->_0 != 0) {
        func_80002A10(D_800488A0, arg0->_0, arg0->_8, arg0->_4 - arg0->_0, 0);
    }
    if (arg0->_20 - arg0->_1C != 0) {
        func_80033BE0(arg0->_1C, arg0->_20 - arg0->_1C);
    }
}
