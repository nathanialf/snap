#include "common.h"

s32 func_8012E03C(s32 *arg0, s32 arg1, s32 arg2) {
    switch (arg1) {
        case 1:
            arg0[0] = arg2;
            break;
        case 6:
            arg0[5] = arg2;
            break;
    }
    return 0;
}
