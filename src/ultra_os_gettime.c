#include "common.h"

extern u32 func_80038D70(void);
extern u32 func_80036010(void);
extern void func_80038D90(u32);
extern u64 D_800994C0;
extern u32 D_800994C8;

#define __osDisableInt    func_80038D70
#define osGetCount        func_80036010
#define __osRestoreInt    func_80038D90
#define __osCurrentTime   D_800994C0
#define __osBaseCounter   D_800994C8

u64 func_80137EE0(void) {
    u32 tmptime;
    u32 elapseCount;
    u64 currentCount;
    register u32 saveMask;

    saveMask = __osDisableInt();
    tmptime = osGetCount();
    elapseCount = tmptime - __osBaseCounter;
    currentCount = __osCurrentTime;
    __osRestoreInt(saveMask);
    return currentCount + elapseCount;
}
