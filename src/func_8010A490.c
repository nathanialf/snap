#include "common.h"

extern void *func_8000A344(s32, s32, u8, s32);
extern void func_8000848C(void *, s32 *);

void *func_8010A490(arg0, arg1, arg2)
s32 arg0;
s32 arg1;
s32 *arg2;
{
    void *p = func_8000A344(arg0, arg1, *((u8 *) arg2 + 0xC), arg2[4]);
    if (p == 0) return 0;
    func_8000848C(p, arg2);
    return p;
}
