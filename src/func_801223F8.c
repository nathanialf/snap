#include "common.h"

extern s32 func_80032980(s32);
extern u8 *D_80096910;
extern s32 *D_80096914;

void func_801223F8(s32 arg0) {
    s32 prev = func_80032980(1);
    D_80096910[arg0] = 1;
    D_80096914[arg0] = -1;
    func_80032980(prev);
}
