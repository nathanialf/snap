#include "common.h"

extern u8 *D_8004AC50;
extern u8 D_80043720;
extern u8 D_8004AC60;

extern void func_80025F24(u8 *, s32);
extern void func_80032630(u8 *, s32, s32);
extern void func_80032780(s32);

void func_8010BCA8(s32 arg0) {
    s32 *p = *(s32 **) ((u8 *) *(u8 **) (D_8004AC50 + 0x1C) + 0x1B8);
    if (p[14] != 0 || p[15] != 0xFEDCBA98) {
        func_80025F24(&D_80043720, **(s32 **) (D_8004AC50 + 0x18));
    }
    while (arg0 != 0) {
        func_80032630(&D_8004AC60, 1, 0);
        func_80032780(0);
        arg0--;
    }
}
