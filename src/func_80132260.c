#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 _pad;
    s32 *ptr;
} ScreenMsg3;

void func_80132260(s32 *arg0) {
    ScreenMsg3 msg;
    s32 *entry = (s32 *) ((u8 *) arg0[0x40 / 4] + arg0[0x3C / 4] * 0x30);
    if (entry[0x28 / 4] == 0) {
        msg.type = 0;
        msg.ptr = (s32 *) ((u8 *) arg0[0x40 / 4] + arg0[0x3C / 4] * 0x30);
        func_80029E80((u8 *) arg0 + 0x14, &msg, 0);
    }
}
