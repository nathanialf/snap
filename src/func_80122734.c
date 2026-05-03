#include "common.h"

extern s32 *D_800968F0[];
extern void func_8002DDE0(s32 *);
extern void func_8002A390(s32 *);

void func_80122734(s32 arg0) {
    func_8002DDE0(D_800968F0[arg0]);
}

void func_80122760(s32 arg0) {
    func_8002A390(D_800968F0[arg0]);
}
