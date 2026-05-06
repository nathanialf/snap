#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 pad;
    s32 ptr;
    f32 fval;
    s32 _unused;
} Msg16;

void func_8012A640(s32 *arg0, f32 arg1) {
    Msg16 msg;
    s32 *base;

    base = (s32 *) arg0[0x10];
    msg.type = 4;
    msg.ptr = (s32) ((u8 *) base + arg0[0xF] * 0x30);
    msg.fval = arg1;
    func_80029E80((u8 *) arg0 + 0x14, &msg, 0);
}
