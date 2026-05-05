#include "common.h"

extern void func_80025F24(const char *);

extern s32 D_8004A910;
extern s32 D_8004A828[];
extern s32 D_8004A830[];
extern s32 D_8004A838[];
extern char D_800430F0[];
extern char D_80043110[];

s32 func_8010584C(void) {
    s32 idx = D_8004A910;
    s32 old;

    if (D_8004A828[idx] == 0) {
        func_80025F24(D_800430F0);
        for (;;) {
        }
    }
    old = D_8004A830[idx];
    if (old == D_8004A838[idx]) {
        func_80025F24(D_80043110);
        for (;;) {
        }
    }
    D_8004A830[idx] = old + 0x88;
    return old;
}
