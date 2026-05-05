#include "common.h"

s32 func_801314EC(s32 *arg0, s32 arg1) {
    register s32 result = 0x3E8;
    s32 diff = arg0[9] - arg1;
    if (diff >= 0) {
        result = diff;
    }
    return result;
}
