#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 halfword;
    s32 *ptr;
    s32 _wc;
} ScreenMsgH;

void func_8012DEB0(arg0, arg1)
s32 *arg0;
s16 arg1;
{
    ScreenMsgH msg;
    msg.type = 3;
    msg.halfword = arg1;
    msg.ptr = (s32 *) ((u8 *) arg0[0x40 / 4] + arg0[0x3C / 4] * 0x30);
    func_80029E80((u8 *) arg0 + 0x14, &msg, 0);
}
