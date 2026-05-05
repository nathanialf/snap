#include "common.h"

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

typedef struct {
    s16 type;
    s16 _pad;
    s32 *entry_ptr;
    s32 _pad2[2];
} EntryMsg;

void func_80132260(s32 *arg0) {
    EntryMsg msg;
    s32 *base = (s32 *) arg0[0x10];
    if (*(s32 *) ((u8 *) base + arg0[0xF] * 0x30 + 0x28) != 0) {
        return;
    }
    msg.type = 0;
    msg.entry_ptr = (s32 *) ((u8 *) base + arg0[0xF] * 0x30);
    func_80029E80((u8 *) arg0 + 0x14, &msg, 0);
}
