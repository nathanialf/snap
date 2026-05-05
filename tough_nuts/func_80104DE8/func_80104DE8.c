#include "common.h"

extern u8 D_800488D0;
extern u8 D_80049730[];
extern u8 D_8004974F;

extern void func_8003B780(u8 *, s32, s32, u8 *);
extern void func_8003B530(u8 *, s32, s32, u8 *, s32);

void func_80104DE8(s32 *arg0) {
    switch (arg0[4]) {
        case 0:
            func_8003B780(&D_800488D0, arg0[3], 0x600, D_80049730);
            arg0[5] = D_8004974F;
            break;
        case 1:
            D_80049730[0x1F] = arg0[5];
            func_8003B530(&D_800488D0, arg0[3], 0x600, D_80049730, 1);
            break;
    }
}
