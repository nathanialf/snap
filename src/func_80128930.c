#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 _pad;
    s32 data;
    s32 _unused[2];
} ScreenMsg;

void func_80128930(s32 *arg0, s32 arg1) {
    ScreenMsg msg;
    msg.type = 0xE;
    msg.data = arg1;
    func_80029E80((u8 *) arg0 + 0x4C, &msg, 0);
}
