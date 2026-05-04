#include "common.h"

extern s16 D_80049798;
extern s16 D_8004979A;
extern s32 D_8004979C;
extern u8 D_800497A8;
extern s32 D_8004A910;
extern s32 D_8004A914[];

extern void func_80032630(u8 *, s32, s32);

typedef struct {
    int x, y;
} Vec2i;

void func_801075FC(Vec2i v) {
    D_80049798 = v.x;
    D_8004979A = v.y;
}

void func_80107618(void) {
    if (D_8004979C != 2) {
        D_8004979C = 1;
    }
}

s32 func_80107640(void) {
    if (D_8004979C == 2) {
        return 1;
    }
    return 0;
}

void func_80107668(void) {
    func_80032630(&D_800497A8, 0, 0);
}

void func_80107694(s32 arg0) {
    if (arg0 == 1 || arg0 == 2) {
        D_8004A910 = arg0;
    }
}

s32 func_801076B4(s32 arg0) {
    if (arg0 == 1 || arg0 == 2) {
        if (D_8004A914[arg0] == 0) {
            return 1;
        }
    }
    return 0;
}
