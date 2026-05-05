#include "common.h"

typedef struct Foo {
    u8 _0[0x3];
    u8 _3;
    u8 _4[0xA];
    s16 _E;
} Foo;

extern void func_8001FFD8(void *, s32, s32);

void func_8011FF40(Foo *arg0, s32 arg1, s32 arg2) {
    if (arg0->_3 == 0) {
        arg0->_3 = 1;
        if (arg0->_E > 0) {
            s32 i = 0;
            s32 *p = (s32 *)arg0;
            do {
                void *r = (void *)(p[4] + arg1);
                p[4] = (s32)r;
                func_8001FFD8(r, arg1, arg2);
                i++;
                p++;
            } while (i < arg0->_E);
        }
    }
}
