#include "common.h"

extern s32 func_80010584(s32);
extern void func_80010E54(s32);

void func_80111090(s32 *arg0) {
    s32 v = arg0[0x12];
    func_80010584(v);
    func_80010E54(v);
}
