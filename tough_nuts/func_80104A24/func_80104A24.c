#include "common.h"

typedef struct {
    s32 _0;
    s32 _4;
    s32 _8;
    s32 _C;
    s32 _10;
    s16 _14;
    s16 _16;
    s32 _18;
    s32 _1C;
    s16 _20;
    s16 _22;
} Slot;

extern Slot D_80048B88[];
extern s32 D_80048940[];
extern s32 D_80048918[];

extern void func_80032840(s32 *, s32 *, s32);
extern void func_80032630(s32 *, s32 *, s32);

void func_80104A24(arg0, arg1, arg2, arg3, arg4)
s32 arg0;
u16 arg1;
u16 arg2;
s32 arg3;
u16 arg4;
{
    Slot *p;
    s32 i;

    for (i = 0, p = D_80048B88; i < 4; i++, p++) {
        if (p->_0 == 0) break;
    }
    if (i == 4) {
        func_80032840(D_80048940, &i, 1);
        p = &D_80048B88[i];
    } else {
        p->_0 = 1;
    }
    p->_10 = arg0;
    p->_14 = arg1;
    p->_18 = arg2;
    p->_1C = arg3;
    p->_20 = arg4;
    func_80032630(D_80048918, &p->_4, 0);
}
