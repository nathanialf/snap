#include "common.h"

extern s32 *D_800497C8;
extern s32 D_8004A8C8;
extern s32 D_8004A900;
extern s16 D_8004A904;
extern s16 D_8004A906;

extern s32 func_800078C4(s32 *, s32, s32, s32);
extern s32 func_8000785C(s32 *, s32, s32);

typedef struct {
    int x, y;
} Vec2i;

void func_80105418(s32 **arg0) {
    D_800497C8 = (s32 *) ((u8 *) *arg0 + 4);
    {
        s32 *p = *arg0;
        *arg0 = (s32 *) ((u8 *) p + 8);
        p[1] = 0;
        p[0] = 0xDB06003C;
    }
}

void func_80105448(s32 arg0) {
    D_8004A900 = arg0;
}

void func_80105454(Vec2i v) {
    D_8004A904 = v.x;
    D_8004A906 = v.y;
}

void func_80105470(s32 arg0, s32 arg1) {
    func_800078C4(&D_8004A8C8, 0x10000, arg0, arg1);
}

void func_801054A0(arg0, arg1)
s32 arg0;
s32 arg1;
{
    func_8000785C(&D_8004A8C8, arg0, arg1);
}
