#include "common.h"

extern s32 D_8004A954;

u32 func_80107910(u32 arg0) {
    u32 result, final;
    if (D_8004A954 == 3) {
        return arg0;
    }
    result = ((arg0 >> 16) & 0xF800) | ((arg0 >> 13) & 0x7C0) | ((arg0 >> 10) & 0x3E) | ((arg0 >> 7) & 1);
    final = (result << 16) | result;
    return final;
}
