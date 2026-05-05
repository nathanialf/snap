#include "common.h"

typedef struct Foo {
    u8 padding[0x80];
    s32 flags;
} Foo;

extern void func_80006330(void);
extern void func_80005F34(void);
extern void func_80005A98(void);

void func_801190E4(Foo *arg0) {
    s32 v;

    v = arg0->flags;
    if (v & 4) {
        func_80006330();
        v = arg0->flags;
    }
    if (v & 0x10) {
        func_80005F34();
        func_80005A98();
        v = arg0->flags;
    }
    if (v & 0x40) {
        func_80005F34();
    }
}
