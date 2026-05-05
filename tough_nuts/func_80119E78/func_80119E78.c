#include "common.h"

extern u64 func_80037EE0(void);
extern f32 func_8003D478(s32, u8);

f32 func_80119E78(void) {
    f32 r = func_8003D478(0, (u8) func_80037EE0());
    return r * (1.0f / 256.0f);
}
