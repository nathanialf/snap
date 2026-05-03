#include "common.h"

extern void *D_8004AC44;
extern void func_8000A9D8(void *, s32, s32, s32, s32);
extern void func_800086B8(void *);
extern void func_80008724(void *);

void func_8010AA0C(void *arg0, s32 a1, s32 a2, s32 a3, s32 a4) {
    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    func_8000A9D8(arg0, a1, a2, a3, a4);
    func_800086B8(arg0);
}

void func_8010AA48(void *arg0, s32 a1, s32 a2, s32 a3, s32 a4) {
    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    func_8000A9D8(arg0, a1, a2, a3, a4);
    func_80008724(arg0);
}
