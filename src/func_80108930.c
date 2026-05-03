#include "common.h"

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
