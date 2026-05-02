#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 _pad0;
    s8  byte_a;
    s8  byte_b;
    s8  _pad1[10];
} ScreenMsg;

void func_80128970(thr, arg1, arg2)
s32 *thr;
char arg1;
int arg2;
{
    ScreenMsg msg;
    msg.type = 12;
    msg.byte_a = arg1;
    msg.byte_b = ((unsigned char *) &arg2)[3];
    func_80029E80((u8 *)thr + 0x4C, &msg, 0);
}
