#include "common.h"

extern s32 D_8004A914[];

s32 func_801076B4(s32 arg0) {
    if (arg0 == 1 || arg0 == 2) {
        if (D_8004A914[arg0] == 0) {
            return 1;
        }
    }
    return 0;
}
