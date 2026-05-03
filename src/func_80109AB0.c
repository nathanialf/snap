#include "common.h"

extern u8 *func_800088B8(s32, s32);
extern void func_80008950(s32, u8 *);

u8 *func_80109AB0(arg0, arg1)
s32 arg0;
int arg1;
{
    u8 *r;
    r = func_800088B8(arg0, arg1);
    r[0x4] = ((u8 *) &arg1)[3];
    r[0x5] = 0;
    *(s32 *) (r + 0x20) = 0;
    *(f32 *) (r + 0x1C) = 0.0f;
    *(f32 *) (r + 0x18) = 0.0f;
    *(f32 *) (r + 0x14) = 0.0f;
    *(f32 *) (r + 0x10) = 0.0f;
    *(f32 *) (r + 0xC) = 0.0f;
    *(f32 *) (r + 0x8) = 1.0f;
    func_80008950(arg0, r);
    return r;
}
