#include "common.h"

extern s32 D_8004A950;
extern void func_80007964(s32, s32, s32);
extern void func_80007B58(s32, s32, s32);

void func_80107BC4(s32 *arg0) {
    func_80007964(arg0[0], arg0[1], arg0[2]);
    D_8004A950 = arg0[3];
    func_80007B58(arg0[4], arg0[5], arg0[6]);
}
