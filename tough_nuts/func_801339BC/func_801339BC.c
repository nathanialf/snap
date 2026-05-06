#include "common.h"

void func_801339BC(s32 **arg0, s32 *arg1) {
    s32 **prev = arg0;
    s32 *cur = *arg0;
    if (cur[1] >= arg1[1]) {
        do {
            prev = (s32 **) cur;
            cur = (s32 *) cur[0];
        } while (cur[1] >= arg1[1]);
    }
    arg1[0] = (s32) cur;
    *prev = arg1;
    arg1[2] = (s32) arg0;
}
