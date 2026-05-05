#include "common.h"

typedef void *OSMesg;

extern s32 D_80048868[];

extern void func_80037700(s32 *, OSMesg *, s32);
extern void func_80032630(s32 *, void *, s32);
extern void func_80032840(s32 *, s32, s32);

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
