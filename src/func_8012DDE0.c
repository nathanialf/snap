#include "common.h"

typedef struct ColorMsg {
    s16 type;
    s16 _2;
    s32 _4;
    u8 _8;
    u8 _9;
    u8 _A;
    u8 _B;
    u8 _C;
    u8 _D;
    u8 _E;
    u8 _F;
} ColorMsg;

extern s32 func_80029E80(void *queue, void *msg, s32 flag);

void func_8012DDE0(s32 arg0, s32 arg1) {
    ColorMsg msg;
    msg.type = 7;
    msg._8 = 0xFF;
    msg._9 = 0x51;
    msg._B = (u8) ((arg1 & 0xFF0000) >> 16);
    msg._C = (u8) ((arg1 & 0xFF00) >> 8);
    msg._D = ((u8) arg1) & 0xFFFFFFFFFFFFFFFF;
    func_80029E80((u8 *) arg0 + 0x4C, &msg, 0);
}
