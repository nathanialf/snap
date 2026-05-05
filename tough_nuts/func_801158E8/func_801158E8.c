#include "common.h"

typedef struct Foo {
    u8 _0[0x1C];
    f32 _1C;
    f32 _20;
    f32 _24;
} Foo;

typedef struct Bar {
    u8 _0[0x3C];
    f32 _3C;
    f32 _40;
    f32 _44;
    struct Bar *_48;
} Bar;

extern Bar *D_8004AC48;

f32 func_801158E8(Foo *arg0) {
    Bar *p = D_8004AC48->_48;
    f32 dx = arg0->_1C - p->_3C;
    f32 dy = arg0->_20 - p->_40;
    f32 dz = arg0->_24 - p->_44;
    return (dx*dx + dy*dy) + dz*dz;
}
