#include "common.h"

typedef struct {
    s32 _0;
    s32 _4;
    s32 _8;
    s32 _C;
    s32 _10;
    s32 _14;
} Foo;

extern s32 D_80048C18[];
extern u8 D_80048C1C;
extern u8 D_80048918;
extern u8 D_800488D0[];
extern u8 D_80049730[];
extern s8 D_80048A03;
extern u8 D_80048C33;
extern u8 D_800489B8;

extern void func_80004D94(s32, s32, s32);
extern void func_80032630(u8 *queue, u8 *msg, s32 flag);
extern void func_80032840(u8 *, s32 *, s32);
extern void func_8003B780(u8 *, s32, s32, u8 *);
extern void func_8003B530(u8 *, s32, s32, u8 *, s32);

void func_80104D94(arg0, arg1, arg2)
s32 arg0;
s32 arg1;
s32 arg2;
{
    D_80048C18[4] = arg0;
    D_80048C18[5] = ((unsigned short *) &arg1)[1];
    D_80048C18[6] = ((unsigned short *) &arg2)[1];
    func_80032630(&D_80048918, &D_80048C1C, 0);
}

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

s32 func_80104E7C(void) {
    if (D_80048A03 == 5) {
        return 1;
    }
    return 0;
}

u8 func_80104EA4(void) {
    s32 local;
    func_80004D94(3, 0, 0);
    func_80032840(&D_800489B8, &local, 1);
    return D_80048C33;
}

void func_80104EE4(arg0)
u8 arg0;
{
    s32 local;
    func_80004D94(3, 1, arg0);
    func_80032840(&D_800489B8, &local, 1);
}
