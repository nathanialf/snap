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

typedef struct {
    s32 _hole[2];
    EntryByteMsg msg;
} Frame;

void func_80132190(arg0, arg1)
s32 *arg0;
u8 arg1;
{
    Frame f;
    f.msg.type = 2;
    f.msg.entry_ptr = (s32 *) ((u8 *) arg0[0x10] + arg0[0xF] * 0x30);
    f.msg.value = arg1;
    func_80029E80((u8 *) arg0 + 0x14, &f.msg, 0);
}
