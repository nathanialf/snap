#include "common.h"

extern void *D_8004AC10;
extern s32 D_8004AC14;

void func_80108930(u8 *arg0, u8 *arg1) {
    *(u8 **) arg1 = *(u8 **) (arg0 + 0x6C);
    *(u8 **) (arg0 + 0x6C) = arg1;
}

void func_80108940(u8 *arg0, u8 *arg1) {
    *(u8 **) arg1 = *(u8 **) (arg0 + 0x90);
    *(u8 **) (arg0 + 0x90) = arg1;
}

void func_80108950(u8 *arg0, u8 *arg1) {
    *(u8 **) arg1 = *(u8 **) (arg0 + 0x6C);
    *(u8 **) (arg0 + 0x6C) = arg1;
}

void func_80108960(void **arg0) {
    arg0[0] = D_8004AC10;
    D_8004AC10 = arg0;
    D_8004AC14--;
}
