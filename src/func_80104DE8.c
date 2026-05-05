#include "common.h"

typedef struct {
    s32 _0;
    s32 _4;
    s32 _8;
    s32 _C;
    s32 _10;
    s32 _14;
} Foo;

extern u8 D_800488D0[];
extern u8 D_80049730[];

extern void func_8003B780(u8 *, s32, s32, u8 *);
extern void func_8003B530(u8 *, s32, s32, u8 *, s32);

void func_80104DE8(Foo *arg0) {
    switch (arg0->_10) {
    case 0:
        func_8003B780(D_800488D0, arg0->_C, 0x600, D_80049730);
        arg0->_14 = D_80049730[0x1F];
        break;
    case 1:
        D_80049730[0x1F] = arg0->_14;
        func_8003B530(D_800488D0, arg0->_C, 0x600, D_80049730, 1);
        break;
    }
}
