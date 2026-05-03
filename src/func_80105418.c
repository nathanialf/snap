#include "common.h"

extern s32 *D_800497C8;

void func_80105418(s32 **arg0) {
    D_800497C8 = (s32 *) ((u8 *) *arg0 + 4);
    {
        s32 *p = *arg0;
        *arg0 = (s32 *) ((u8 *) p + 8);
        p[1] = 0;
        p[0] = 0xDB06003C;
    }
}
