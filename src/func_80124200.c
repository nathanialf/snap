#include "common.h"

extern void func_80024054(s32, s32, s32, s32, s32, s32);

void func_80124200(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    func_80024054(arg0, arg1, arg2, arg3, 0, arg4);
}
