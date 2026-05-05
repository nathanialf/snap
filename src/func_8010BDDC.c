#include "common.h"

extern u8 *D_8004AC44;

void func_8010BDDC(u8 *arg0, s32 arg1) {
    s32 *cur;
    if (arg0 == 0) arg0 = D_8004AC44;
    cur = *(s32 **) (arg0 + 0x18);
    if (cur != 0) {
        do {
            if (cur[8] == arg1) {
                *((u8 *) cur + 0x15) = 1;
            }
        } while ((cur = (s32 *) cur[0]) != 0);
    }
}

void func_8010BE1C(u8 *arg0, s32 arg1) {
    s32 *cur;
    if (arg0 == 0) arg0 = D_8004AC44;
    cur = *(s32 **) (arg0 + 0x18);
    if (cur != 0) {
        do {
            if (cur[8] == arg1) {
                *((u8 *) cur + 0x15) = 0;
            }
        } while ((cur = (s32 *) cur[0]) != 0);
    }
}
