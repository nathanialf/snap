#include "common.h"

extern s32 D_80042CD0;
extern s32 D_80097E98;

extern void func_80034600(void);
extern void func_80032840(s32 *, s32 *, s32);

void func_80134650(void) {
    s32 local;
    if (D_80042CD0 == 0) {
        func_80034600();
    }
    func_80032840(&D_80097E98, &local, 1);
}
