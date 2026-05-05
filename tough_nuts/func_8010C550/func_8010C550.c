#include "common.h"

void *func_8010C550(s32 *arg0) {
    if (arg0[4] != 0) return (void *) arg0[4];
    if (arg0[2] != 0) return (void *) arg0[2];
loop:
    {
        s32 *v = (s32 *) arg0[5];
        if ((s32) v == 1) return 0;
        if (v[2] != 0) return (void *) v[2];
        arg0 = v;
    }
    goto loop;
}
