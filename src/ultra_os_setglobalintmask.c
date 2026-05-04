#include "common.h"

extern u32 func_80038D70(void);
extern void func_80038D90(u32);
extern u32 D_80042D00;

#define __osDisableInt    func_80038D70
#define __osRestoreInt    func_80038D90
#define __OSGlobalIntMask D_80042D00

void func_80138370(u32 mask) {
    register u32 saveMask = __osDisableInt();

    __OSGlobalIntMask |= mask;

    __osRestoreInt(saveMask);
}
