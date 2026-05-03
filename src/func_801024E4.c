#include "common.h"

extern void func_80000C74(s32 *);
extern void func_80001D94(void);

void func_801024E4(s32 *arg0) {
    arg0[2] = 1;
    func_80000C74(arg0);
    func_80001D94();
}
