#include "common.h"

extern void *D_8004AC44;
extern void func_8000A828(void *, s32, u8, s32, s32);
extern void func_800086B8(void *);
extern void func_80008724(void *);

void func_8010A8A4(arg0, arg1, arg2, arg3, arg4)
void *arg0;
s32 arg1;
unsigned char arg2;
s32 arg3;
s32 arg4;
{
    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    func_8000A828(arg0, arg1, arg2, arg3, arg4);
    func_800086B8(arg0);
}

void func_8010A8E8(arg0, arg1, arg2, arg3, arg4)
void *arg0;
s32 arg1;
unsigned char arg2;
s32 arg3;
s32 arg4;
{
    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    func_8000A828(arg0, arg1, arg2, arg3, arg4);
    func_80008724(arg0);
}
