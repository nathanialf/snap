#include "common.h"

extern s32 func_80029414(s32, s32);

s32 func_801294D4(s32 arg0, s32 arg1) {
    s32 byte;
    s32 result;

    byte = func_80029414(arg0, arg1);
    result = byte;
    if (byte & 0x80) {
        result = byte & 0x7F;
        do {
            byte = func_80029414(arg0, arg1);
            result = (result << 7) + (byte & 0x7F);
        } while (byte & 0x80);
    }
    return result;
}
