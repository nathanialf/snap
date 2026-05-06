#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 _pad;
    s32 *entry_ptr;
    u8 value;
    u8 _pad2[3];
    s32 _pad3;
} EntryByteMsg;

void func_8012A460(arg0, arg1)
s32 *arg0;
u8 arg1;
{
    EntryByteMsg msg;
    s32 *base;

    base = (s32 *) arg0[0x10];
    msg.type = 8;
    msg.entry_ptr = (s32 *) ((u8 *) base + arg0[0xF] * 0x30);
    msg.value = arg1;
    func_80029E80((u8 *) arg0 + 0x14, &msg, 0);
}
