#include "common.h"

typedef struct Foo {
    s32 _0;
    s32 _4;
    u8 _8[0xC];
    s32 _14;
    u8 _18[0x4];
    s32 _1C;
    s32 _20;
    s32 _24;
    s32 _28;
} Foo;

extern void func_80032630(u8 *, s32, s32);
extern u8 D_80048868;
extern s32 D_8004A910;

void func_801059B0(Foo *arg0, s32 arg1, s32 arg2, s32 arg3) {
    arg0->_0 = 6;
    arg0->_4 = 100;
    arg0->_14 = 0;
    arg0->_20 = arg3;
    arg0->_1C = arg2;
    arg0->_24 = arg1;
    arg0->_28 = D_8004A910;
    func_80032630(&D_80048868, (s32)arg0, 0);
}
