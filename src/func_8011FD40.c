#include "common.h"

void func_8011FD40(s32 *arg0, s32 arg1, s32 arg2) {
    s32 padding = 0x10 - (arg1 & 0xF);
    if (padding != 0x10) {
        arg0[0] = arg1 + padding;
    } else {
        arg0[0] = arg1;
    }
    arg0[2] = arg2;
    arg0[3] = 0;
    arg0[1] = arg0[0];
}
