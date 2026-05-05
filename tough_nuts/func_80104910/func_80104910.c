#include "common.h"

typedef struct {
    s32 _0;
    s32 _4;
    s32 _8;
    s32 _C;
    s32 _10;
    s32 _14;
} Slot;

extern Slot D_80048AC8[];
extern s32 D_80048968[];
extern s32 D_80048918[];

extern void func_80032840(s32 *, s32 *, s32);
extern void func_80032630(s32 *, s32 *, s32);

void func_80104910(s32 arg0, s32 arg1) {
    Slot *p;
    s32 i;

    for (p = D_80048AC8, i = 0; i < 4; p++, i++) {
        if (p->_0 == 0) {
            break;
        }
    }
    if (i == 4) {
        func_80032840(D_80048968, &i, 1);
        p = &D_80048AC8[i];
    } else {
        p->_0 = 1;
    }
    p->_10 = arg0;
    p->_14 = arg1;
    func_80032630(D_80048918, &p->_4, 0);
}
