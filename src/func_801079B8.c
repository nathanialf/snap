#include "common.h"

extern s32 D_8004A950;
extern s32 D_8004A954;
extern s32 D_8004A958;
extern s32 D_8004A95C;
extern s32 D_8004A960;
extern s32 D_8004A964;
extern s16 D_8004A974;
extern s16 D_8004A976;
extern s16 D_8004A978;
extern s16 D_8004A97A;
extern s32 D_80048868;

extern void func_800079B8(s32);
extern void func_80007964(s32, s32, s32);
extern void func_80007A04(s32);
extern void func_80007A1C(s32);
extern void func_80007A94(s32 *);
extern void func_80007B58(s32, s32, s32);
extern void func_800009A0(s32 *);
extern void func_80032630(s32 *, s32 *, s32);

void func_801079B8(s32 arg0) {
    D_8004A960 |= arg0;
    if (arg0 & 0x20) {
        D_8004A954 = 3;
    }
    if (arg0 & 0x10) {
        D_8004A954 = 2;
    }
    D_8004A964 = 1;
}

void func_80107A04(s32 arg0) {
    D_8004A958 = arg0;
    D_8004A964 = 1;
}

void func_80107A1C(s32 arg0) {
    D_8004A95C = arg0;
    D_8004A964 = 1;
}

void func_80107A34(arg0, arg1, arg2, arg3)
s16 arg0;
s16 arg1;
s16 arg2;
s16 arg3;
{
    D_8004A974 = arg0;
    D_8004A976 = arg1;
    D_8004A978 = arg2;
    D_8004A97A = arg3;
    D_8004A964 = 1;
}

void func_80107A94(u8 *arg0) {
    *(s32 *) (arg0 + 0x24) = D_8004A958;
    *(s32 *) (arg0 + 0x28) = D_8004A95C;
    *(s32 *) (arg0 + 0x2C) = D_8004A960;
    *(s16 *) (arg0 + 0x30) = D_8004A974;
    *(s16 *) (arg0 + 0x32) = D_8004A976;
    *(s16 *) (arg0 + 0x34) = D_8004A978;
    *(s16 *) (arg0 + 0x36) = D_8004A97A;
    D_8004A960 = 0;
    D_8004A964 = 0;
}

void func_80107AFC(s32 *arg0) {
    if (D_8004A964 != 0) {
        arg0[0] = 4;
        arg0[1] = 0x32;
        arg0[5] = 0;
        arg0[8] = 0;
        func_80007A94(arg0);
        func_80032630(&D_80048868, arg0, 0);
    }
}

void func_80107B58(s32 arg0, s32 arg1, s32 arg2) {
    s32 buf[14];

    D_8004A960 = 0;
    D_8004A954 = 2;
    func_800079B8(arg2);
    func_80007A04(arg0);
    func_80007A1C(arg1);
    buf[0] = 4;
    buf[1] = 0x64;
    func_80007A94(buf);
    func_800009A0(buf);
}

void func_80107BC4(s32 *arg0) {
    func_80007964(arg0[0], arg0[1], arg0[2]);
    D_8004A950 = arg0[3];
    func_80007B58(arg0[4], arg0[5], arg0[6]);
}
