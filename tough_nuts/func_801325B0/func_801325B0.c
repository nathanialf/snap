#include "common.h"

extern void *func_8002AB70(void *, void *);

void func_801325B0(arg0, arg1)
s32 *arg0;
s32 *arg1;
{
    s32 *r;
    if (arg1[2] != 0) {
        r = (s32 *) func_8002AB70(arg0, arg1);
        if (r != 0) {
            *(s16 *) ((u8 *) r + 8) = 0xF;
            r[0] = 0;
            r[1] = arg0[7] + ((s32 *) arg1[2])[0x36];
            ((void (*)(s32 *, s32, s32 *)) ((s32 *) ((s32 *) arg1[2])[3])[2])
                ((s32 *) ((s32 *) arg1[2])[3], 3, r);
        }
    }
}
