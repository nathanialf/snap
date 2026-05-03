#include "common.h"

extern u8 *D_8004AC50;

void func_8010BDAC(u8 *arg0) {
    if (arg0 == 0) {
        arg0 = D_8004AC50;
    }
    arg0[0x15] = 1;
}

void func_8010BDC4(u8 *arg0) {
    if (arg0 == 0) {
        arg0 = D_8004AC50;
    }
    arg0[0x15] = 0;
}
