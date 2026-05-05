#include "common.h"

typedef struct {
    s16 type;
    s16 pad0;
    s32 data;
    s32 unused[2];
} ScreenMsg;

extern void func_80031B90(s32 *, s16 *);
extern void func_800308EC(s32 *);
extern void func_8003075C(s32 *);
extern void func_80031188(s32 *);
extern s32 func_80029E80(void *queue, void *msg, s32 flag);

void func_80130838(s32 *arg0) {
    ScreenMsg msg;
    s32 *p = (s32 *) arg0[6];
    if (p != 0) {
        func_80031B90(p, &msg.type);
        switch (msg.type) {
            case 1:
                func_800308EC(arg0);
                func_8003075C(arg0);
                break;

            case 3:
                func_80031188(arg0);
                func_8003075C(arg0);
                break;

            case 4:
                if (((!(&msg)) && (!(&msg))) && (!(&msg))) {
                }
                arg0[11] = 2;
                msg.type = 0x10;
                func_80029E80(((u8 *) arg0) + 0x4C, &msg, 0x7FFFFFFF);
                break;
        }
    }
}
