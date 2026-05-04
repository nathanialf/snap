#include "common.h"

extern void *D_800374B0;

extern s32 func_80036D00(void *, char *, const char *, void *);

s32 func_801374D4(char *arg0, const char *arg1, ...) {
    s32 *va = ((s32 *) &arg1) + 1;
    s32 result = func_80036D00(&D_800374B0, arg0, arg1, va);
    if (result >= 0) {
        arg0[result] = 0;
    }
    return result;
}
