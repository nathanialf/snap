#include "common.h"

typedef void *OSMesg;

extern s32 D_8004873C;
extern s32 D_80048754;
extern s32 D_8004874C;
extern s32 D_80048868[];

extern void func_80000920(void);
extern void func_800009A0(s32 *);
extern void func_80037700(void *, void *, s32);
extern void func_80032630(s32 *, void *, s32);
extern void func_80032840(s32 *, s32, s32);

void func_80100920(void) {
}

void func_80100928(void) {
loop:
    if (D_8004873C != 0) {
        goto call;
    }
    if (D_80048754 != 0) {
        goto call;
    }
    if (D_8004874C == 0) {
        return;
    }
call:
    func_80000920();
    goto loop;
}

void func_801009A0(s32 *arg0) {
    OSMesg msg;
    s32 mq[6];

    func_80037700(mq, &msg, 1);
    arg0[5] = 0;
    arg0[7] = 1;
    arg0[8] = (s32) mq;
    func_80032630(D_80048868, arg0, 0);
    func_80032840(mq, 0, 1);
}

void func_80100A08(s32 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 buf[10];

    func_80037700((void *) arg1, (void *) arg2, arg3);
    arg0[1] = arg1;
    buf[0] = 3;
    buf[1] = 0x64;
    buf[9] = (s32) arg0;
    func_800009A0(buf);
}
