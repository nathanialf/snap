#include "common.h"

extern void func_800087AC(void *);
extern void func_80008650(void *, s32);

void func_8010AD14(arg0, arg1)
void *arg0;
s32 *arg1;
{
    func_800087AC(arg0);
    *(s32 *) ((u8 *) arg0 + 0x28) = arg1[10];
    func_80008650(arg0, (s32) arg1);
}

void func_8010AD4C(arg0, arg1)
void *arg0;
s32 *arg1;
{
    func_800087AC(arg0);
    *(s32 *) ((u8 *) arg0 + 0x28) = arg1[10];
    func_80008650(arg0, arg1[2]);
}
