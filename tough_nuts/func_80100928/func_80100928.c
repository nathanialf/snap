#include "common.h"

extern s32 D_8004873C;
extern s32 D_80048754;
extern s32 D_8004874C;

extern void func_80000920(void);

void func_80100928(void) {
    s32 *p0 = &D_8004873C;
    s32 *p1 = &D_80048754;
    s32 *p2 = &D_8004874C;

    while (*p0 != 0 || *p1 != 0 || *p2 != 0) {
        func_80000920();
    }
}
