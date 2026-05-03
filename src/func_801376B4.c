#include "common.h"

u8 *func_801376B4(u8 *arg0, s32 arg1) {
    u8 c = (u8) arg1;
    if (*arg0 != c) {
        do {
            if (*arg0 == 0) {
                return (u8 *) 0;
            }
            arg0++;
        } while (*arg0 != c);
    }
    return arg0;
}
