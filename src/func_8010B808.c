#include "common.h"

extern s32 func_8000B774(s32, s32, s32, s32 *);

void func_8010B808(s32 arg0, s32 *arg1) {
    func_8000B774(arg0, arg1[1], arg1[0], arg1);
}
