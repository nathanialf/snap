#include "common.h"

void func_80131A54(void *arg0, f32 arg1) {
    f32 spill;
    void *ptr = *(void **) ((u8 *) arg0 + 0x18);
    if (ptr != 0) {
        *(s32 *) ((u8 *) arg0 + 0x24) =
            (s32) (*(f32 *) ((u8 *) ptr + 0x14) * (spill = arg1));
    } else {
        *(s32 *) ((u8 *) arg0 + 0x24) = 0x1E8;
    }
}
