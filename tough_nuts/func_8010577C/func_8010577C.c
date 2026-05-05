#include "common.h"

extern u8 D_80048830[];
extern u8 D_800430A4[];

extern void func_800009A0(s32 *);
extern void func_80025F24(u8 *, ...);

void func_8010577C(s32 arg0, s32 arg1) {
    s32 buf[11];

    buf[0] = 8;
    buf[1] = 0x32;
    buf[9] = arg0;
    buf[10] = arg1;
    func_800009A0(buf);

    if ((u32) D_80048830 & 7) {
        func_80025F24(D_800430A4);
        while (1) {}
    }
}
