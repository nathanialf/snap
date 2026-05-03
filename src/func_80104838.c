#include "common.h"

extern void func_800047D4(s32 *);

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
