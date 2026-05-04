#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 _h2;
    s32 *ptr;
    f32 fval;
    s32 _wc;
} ScreenMsgF;

void func_8012A640(s32 *arg0, f32 arg1) {
    ScreenMsgF msg;
    msg.type = 4;
    msg.fval = arg1;
    msg.ptr = (s32 *) ((u8 *) arg0[0x40 / 4] + arg0[0x3C / 4] * 0x30);
    func_80029E80((u8 *) arg0 + 0x14, &msg, 0);
}
