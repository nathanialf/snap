#include "common.h"

typedef struct Foo {
    u8 _0[0x14];
    s32 _14;
    u8 _18[0x24];
    s32 _3C;
    u8 *_40;
} Foo;

typedef struct Msg {
    s16 type;
    s16 pad;
    u8 *ptr;
    s32 _8;
    s32 _C;
    s32 _10;
    s32 _14;
} Msg;

extern void func_80029E80(s32 *, Msg *, s32);

void func_801289C0(Foo *arg0) {
    Msg msg;

    msg.type = 1;
    msg.ptr = arg0->_40 + arg0->_3C * 48;
    func_80029E80(&arg0->_14, &msg, 0);
}
