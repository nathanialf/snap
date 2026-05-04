#include "common.h"

extern u8 D_8004352C;
extern void func_80025F24(u8 *, s32, s32);
extern void func_800087AC(void *);
extern void func_800086B8(void *);

void func_8010AB30(arg0, arg1, arg2)
void *arg0;
unsigned char arg1;
s32 arg2;
{
    if (arg1 >= 0x20) {
        func_80025F24(&D_8004352C, arg1, *(s32 *) arg0);
        while (1) {}
    }
    func_800087AC(arg0);
    *(u8 *) ((u8 *) arg0 + 0xD) = arg1;
    *(s32 *) ((u8 *) arg0 + 0x28) = arg2;
    func_800086B8(arg0);
}
