#include "common.h"

extern s32 D_80042D40;
extern s32 D_80099408;

extern void func_800359E0(void);
extern void func_80032840(s32 *, s32 *, s32);

void func_80135A30(void) {
    s32 local;
    if (D_80042D40 == 0) {
        func_800359E0();
    }
    func_80032840(&D_80099408, &local, 1);
}
