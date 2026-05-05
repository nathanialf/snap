#include "common.h"

void func_80118CDC(s32 *arg0) {
    void (*fn)(void) = (void (*)(void)) arg0[0x22];

    if (fn != 0) {
        fn();
    }
}
