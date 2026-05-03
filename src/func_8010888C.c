#include "common.h"

extern void *D_8004AC04;
extern s32 D_8004AC08;

void func_8010888C(void **arg0) {
    arg0[0] = D_8004AC04;
    D_8004AC04 = arg0;
    D_8004AC08--;
}
