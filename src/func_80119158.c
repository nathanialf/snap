#include "common.h"

typedef struct Bar {
    u8 _0[0x48];
    struct Bar *_48;
    u8 _4C[0x34];
    s32 _80;
} Bar;

extern void func_80017ADC(s32, Bar *);
extern void func_8001837C(s32, Bar *);
extern void func_80018CDC(Bar *, s32);
extern void func_80018FF0(Bar *, s32);
extern void func_800190E4(Bar *);

void func_80119158(Bar *arg0, s32 arg1, s32 arg2) {
    Bar *p;
    s32 v;

    p = arg0->_48;
    func_80017ADC(arg1, p);
    func_8001837C(arg1, p);
    func_80018CDC(p, arg2);
    if (p->_80 & 8) {
        v = 1;
    } else {
        v = 0;
    }
    func_80018FF0(arg0, v);
    func_800190E4(p);
}
