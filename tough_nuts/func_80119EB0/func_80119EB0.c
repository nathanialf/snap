#include "common.h"

extern u64 func_80037EE0(void);

s32 func_80119EB0(arg0)
s32 arg0;
{
    return (s32) ((f32) ((s32) ((u8) func_80037EE0() * arg0)) * (1.0f / 256.0f));
}
