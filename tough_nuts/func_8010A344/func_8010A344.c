#include "common.h"

typedef struct Foo {
    s32 _0;
    s32 _4;
    s32 _8;
    u8 _C;
    u8 _D;
    u8 _E;
    u8 _F;
    s32 _10;
    s32 _14;
    s32 _18;
    s32 _1C;
    s32 _20[4];
    s32 _30;
    s32 _34;
    s32 _38;
    s32 _3C;
    s32 _40;
    s32 _44;
    s32 _48;
    f32 _4C;
    s32 _50;
    s32 _54;
    s32 _58;
} Foo;

extern u8 D_80043478[];

extern void func_80025F24(u8 *, ...);
extern Foo *func_80008394(void);

Foo *func_8010A344(arg0, arg1, arg2, arg3)
s32 arg0;
s32 arg1;
u8 arg2;
s32 arg3;
{
    Foo *v;
    Foo *ret;

    if (arg2 >= 0x20) {
        func_80025F24(D_80043478, arg2, arg0);
        while (1) {}
    }
    v = func_80008394();
    ret = v;
    if (v == 0) {
        return 0;
    }
    v->_C = arg2;
    v->_0 = arg0;
    v->_10 = arg3;
    v->_18 = 0;
    v->_1C = 0;
    v->_44 = 0;
    v->_3C = 0;
    v->_40 = 0;
    v->_50 = 0;
    v->_F = 0;
    v->_48 = 0;
    v->_D = 0x21;
    v->_4C = 0.0f;
    v->_14 = arg1;
    ret->_54 = 0;
    ret->_58 = 0;
    return ret;
}
