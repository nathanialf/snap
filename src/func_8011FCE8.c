#include "common.h"

extern f32 func_8001FA94(s32, f32);
extern void func_8001F770(s32, s32, f32);

void func_8011FCE8(s32 arg0, s32 arg1, f32 arg2) {
    f32 v = func_8001FA94(arg1, arg2);
    func_8001F770(arg0, arg1, v);
}
