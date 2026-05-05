#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 _pad;
    s32 ptr;
    s32 _unused0;
    s32 _unused1;
} Msg;

void func_801289C0(s32 *arg0) {
    Msg msg;
    msg.type = 1;
    msg.ptr = (s32) ((u8 *) arg0[16] + arg0[15] * 48);
    func_80029E80((u8 *) arg0 + 0x14, &msg, 0);
}
