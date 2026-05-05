#include "common.h"

extern s32 D_8004873C;
extern s32 D_80048754;
extern s32 D_8004874C;

extern void func_80000920(void);

void func_80100928(void) {
loop:
    if (D_8004873C != 0) {
        goto call;
    }
    if (D_80048754 != 0) {
        goto call;
    }
    if (D_8004874C == 0) {
        return;
    }
call:
    func_80000920();
    goto loop;
}
