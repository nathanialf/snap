#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 _pad0;
    s32 zero;
    s8  byte_a;
    s8  byte_b;
    s8  byte_c;
    s8  _pad[5];
} ScreenMsg;

s32 func_8012E03C(s32 *arg0, s32 arg1, s32 arg2) {
    switch (arg1) {
        case 1:
            arg0[0] = arg2;
            break;
        case 6:
            arg0[5] = arg2;
            break;
    }
    return 0;
}

void func_8012E070(thr, arg1, arg2)
s32 *thr;
char arg1;
int arg2;
{
    ScreenMsg msg;
    msg.type = 2;
    msg.zero = 0;
    msg.byte_a = arg1 | 0xB0;
    msg.byte_b = 0x07;
    msg.byte_c = ((unsigned char *) &arg2)[3];
    func_80029E80((u8 *)thr + 0x4C, &msg, 0);
}

void func_8012E0CC(thr, arg1, arg2)
s32 *thr;
char arg1;
int arg2;
{
    ScreenMsg msg;
    msg.type = 2;
    msg.zero = 0;
    msg.byte_a = arg1 | 0xB0;
    msg.byte_b = 0x08;
    msg.byte_c = ((char *) &arg2)[3];
    func_80029E80((u8 *)thr + 0x4C, &msg, 0);
}
