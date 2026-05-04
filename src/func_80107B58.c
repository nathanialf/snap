#include "common.h"

extern s32 D_8004A954;
extern s32 D_8004A960;

extern void func_800079B8(s32);
extern void func_80007A04(s32);
extern void func_80007A1C(s32);
extern void func_80007A94(s32 *);
extern void func_800009A0(s32 *);

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
