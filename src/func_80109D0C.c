#include "common.h"

typedef struct Foo {
    u8 _0[0x4C];
    s32 _4C;
    u8 _50[0x4];
    u8 _54;
    u8 _55;
    u8 _56;
    u8 _57;
    s32 _58;
    s32 arr[4];
    s32 _6C;
    s32 _70;
    f32 _74;
    f32 _78;
    f32 _7C;
    s32 _80;
    s32 _84;
} Foo;

extern f32 D_800436F8;

s32 func_80109D0C(Foo *arg0) {
    register s32 i = 1;
    arg0->_4C = 0;
    arg0->_54 = 0;
    arg0->_55 = 0;
    arg0->_56 = 0;
    arg0->_58 = 0;
    arg0->arr[i] = 0;
    arg0->arr[i + 1] = 0;
    arg0->arr[i + 2] = 0;
    arg0->arr[i - 1] = 0;
    arg0->_6C = 0;
    arg0->_70 = 0;
    arg0->_74 = D_800436F8;
    arg0->_78 = 1.0f;
    arg0->_7C = 0.0f;
    arg0->_80 = 0;
    arg0->_84 = 0;
    return i;
}
