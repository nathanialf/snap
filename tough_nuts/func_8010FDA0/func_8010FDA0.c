#include "common.h"

extern void func_80009760(s32, s32, s32);

void func_8010FDA0(arg0, arg1, arg2, arg3)
s32 *arg0;
int arg1;
int arg2;
int arg3;
{
    if (arg1 & 0xFF) {
        func_80009760((s32) arg0, arg1 & 0xFF, 0);
    }
    if (((unsigned char *) &arg2)[3]) {
        func_80009760((s32) arg0, ((unsigned char *) &arg2)[3], 0);
    }
    if (((unsigned char *) &arg3)[3]) {
        func_80009760((s32) arg0, ((unsigned char *) &arg3)[3], 0);
    }
}
