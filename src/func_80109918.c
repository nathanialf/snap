#include "common.h"

extern u8 *func_800088B8(s32, s32);
extern void func_80008930(s32, u8 *);
extern void func_80008940(s32, u8 *);
extern void func_80008950(s32, u8 *);
extern void func_80008960(void *);

extern f32 D_800436E8;
extern f32 D_800436EC;

u8 *func_80109918(arg0, arg1)
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
    func_80008930(arg0, r);
    return r;
}

void func_80109984(s32 *arg0) {
    void *p = (void *) arg0[0x1B];
    while (p != 0) {
        void *next = *(void **) p;
        func_80008960(p);
        p = next;
    }
    arg0[0x1B] = 0;
    *(f32 *) ((u8 *) arg0 + 0x74) = D_800436E8;
}

u8 *func_801099E4(arg0, arg1)
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
    func_80008940(arg0, r);
    return r;
}

void func_80109A50(s32 *arg0) {
    void *p = (void *) arg0[0x24];
    while (p != 0) {
        void *next = *(void **) p;
        func_80008960(p);
        p = next;
    }
    arg0[0x24] = 0;
    *(f32 *) ((u8 *) arg0 + 0x98) = D_800436EC;
}

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
