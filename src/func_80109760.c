#include "common.h"

extern void func_80008FFC(u8 *, u8, u8, u8);

void func_80109760(arg0, arg1, arg2)
u8 *arg0;
u8 arg1;
u8 arg2;
{
    func_80008FFC(arg0, arg1, arg2, arg0[0x56]);
}
