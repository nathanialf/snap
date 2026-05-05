#include "common.h"

extern void (*D_8004A948)(s32 *);

extern s32 func_80006804(void);
extern void func_8000C274(void);

void func_80106EC0(s32 *arg0) {
    D_8004A948(arg0);
    ((void (*)(void)) arg0[1])();
    if (func_80006804() != 0) {
        func_8000C274();
    }
}
