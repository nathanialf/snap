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
