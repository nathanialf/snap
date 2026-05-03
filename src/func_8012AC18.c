#include "common.h"

extern void func_80029CA0(s32);
extern s32 func_80029C7C(s32, s32 *);

void func_8012AC18(s32 *arg0, s32 arg1) {
    func_80029CA0(arg1);
    func_80029C7C(arg1, arg0 + 5);
}
