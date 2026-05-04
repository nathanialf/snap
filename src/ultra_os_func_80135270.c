#include "common.h"

extern u32 func_80038D70(void);
extern void func_80038D90(u32);
extern s32 *D_80042EB4;

#define __osDisableInt func_80038D70
#define __osRestoreInt func_80038D90

s32 func_80135270(void) {
    register u32 saveMask = __osDisableInt();
    s32 result = D_80042EB4[1];
    __osRestoreInt(saveMask);
    return result;
}
