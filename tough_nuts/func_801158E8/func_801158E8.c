#include "common.h"

typedef struct Foo {
    u8 _0[0x1C];
    f32 x;
    f32 y;
    f32 z;
} Foo;

typedef struct Inner {
    u8 _0[0x3C];
    f32 x;
    f32 y;
    f32 z;
} Inner;

typedef struct Bar {
    u8 _0[0x48];
    Inner *inner;
} Bar;

extern Bar *D_8004AC48;

f32 func_801158E8(Foo *arg0) {
    Inner *p = D_8004AC48->inner;
    f32 dx = arg0->x - p->x;
    f32 dy = arg0->y - p->y;
    f32 dz = arg0->z - p->z;
    return (dx * dx + dy * dy) + dz * dz;
}
