#include "common.h"

extern void *D_8004AC44;
extern s32 D_8000B808;

extern s32 func_8000BAA4(s32, s32 *, void *, s32);

void func_8010B830(s32 arg0, s32 arg1, void *arg2) {
    void *buf[2];
    if (arg2 != 0) {
        buf[0] = arg2;
    } else {
        buf[0] = D_8004AC44;
    }
    buf[1] = (void *) arg1;
    func_8000BAA4(arg0, &D_8000B808, buf, 0);
}
