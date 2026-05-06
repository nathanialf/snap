#include "common.h"

typedef void *OSMesg;

extern s32 D_80048918[];

extern void func_800047D4(s32 *);
extern void func_80037700(s32 *, OSMesg *, s32);
extern void func_80032630(s32 *, void *, s32);
extern void func_80032840(s32 *, s32, s32);

void func_801047D4(s32 *arg0) {
    OSMesg msg;
    s32 mq[6];

    func_80037700(mq, &msg, 1);
    arg0[1] = 1;
    arg0[2] = (s32) mq;
    func_80032630(D_80048918, arg0, 0);
    func_80032840(mq, 0, 1);
}

void func_80104838(void) {
    s32 buf[3];
    buf[0] = 1;
    func_800047D4(buf);
}

void func_80104860(void) {
    s32 buf[3];
    buf[0] = 2;
    func_800047D4(buf);
}

void func_80104888(s32 arg0, s32 arg1) {
    s32 buf[5];
    buf[0] = 3;
    buf[3] = arg0;
    buf[4] = arg1;
    func_800047D4(buf);
}

void func_801048B8(s32 arg0) {
    s32 buf[4];
    buf[0] = 4;
    buf[3] = arg0;
    func_800047D4(buf);
}

void func_801048E4(s32 arg0) {
    s32 buf[4];
    buf[0] = 6;
    buf[3] = arg0;
    func_800047D4(buf);
}
