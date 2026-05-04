#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 _h2;
    s32 *ptr;
    u8 byte;
    u8 _b9[3];
    s32 _wc;
} ScreenMsg2;

void func_8012A460(arg0, arg1)
s32 *arg0;
s32 arg1;
{
    ScreenMsg2 msg;
    msg.type = 8;
    msg.byte = ((u8 *) &arg1)[3];
    msg.ptr = (s32 *) ((u8 *) arg0[0x40 / 4] + arg0[0x3C / 4] * 0x30);
    func_80029E80((u8 *) arg0 + 0x14, &msg, 0);
}
