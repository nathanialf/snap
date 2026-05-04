#include "common.h"

extern u32 func_80038D70(void);
extern void func_80038D90(u32);
extern s32 D_80042D2C;

s32 func_80135610(s32 *arg0) {
    u32 saveMask = func_80038D70();
    *arg0 = D_80042D2C;
    D_80042D2C = (s32) arg0;
    func_80038D90(saveMask);
    return 0;
}
