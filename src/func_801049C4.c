#include "common.h"

extern void func_80004910(s32, s32);

void func_801049C4(s32 arg0) {
    func_80004910(arg0, 1);
}

void func_801049E4(s32 arg0) {
    func_80004910(arg0, 2);
}

void func_80104A04(s32 arg0) {
    func_80004910(arg0, 0);
}
