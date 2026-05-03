#include "common.h"

extern s32 func_8002AC50(void);

s32 func_8012AC9C(void) {
    return func_8002AC50() & ~0xF;
}
