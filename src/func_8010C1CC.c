#include "common.h"

extern s32 *D_8004AC44;
extern void func_80009F2C(void *);

void func_8010C1CC(s32 *arg0) {
    void *cur;
    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    while ((cur = (void *) arg0[0x12]) != 0) {
        func_80009F2C(cur);
    }
}
