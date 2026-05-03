#include "common.h"

extern u8 *D_80048734;
extern u8 *D_80048738;

void func_80100C74(u8 *arg0) {
    u8 *cur;
    s32 key;

    cur = D_80048738;
    if (cur != 0) {
        key = *(s32 *) (arg0 + 4);
        if (*(s32 *) (cur + 4) < key) {
            do {
                cur = *(u8 **) (cur + 0x10);
            } while (cur != 0 && *(s32 *) (cur + 4) < key);
        }
    }

    *(u8 **) (arg0 + 0x10) = cur;
    if (cur != 0) {
        *(u8 **) (arg0 + 0xC) = *(u8 **) (cur + 0xC);
        *(u8 **) (cur + 0xC) = arg0;
    } else {
        *(u8 **) (arg0 + 0xC) = D_80048734;
        D_80048734 = arg0;
    }

    {
        u8 *prev = *(u8 **) (arg0 + 0xC);
        if (prev != 0) {
            *(u8 **) (prev + 0x10) = arg0;
        } else {
            D_80048738 = arg0;
        }
    }
}
