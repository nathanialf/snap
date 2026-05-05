#include "common.h"

extern s32 D_8004A8B0;
extern s32 D_80048814;

extern void func_800009A0(void *);

s32 func_80106804(void) {
    s32 buf[9];
    switch (D_8004A8B0) {
        case 1:
            return 1;
        case 2:
            if (D_80048814 != 0) {
                buf[0] = 0xB;
                buf[1] = 0x64;
                func_800009A0(buf);
                return 1;
            }
            return 0;
    }
    return 0;
}
