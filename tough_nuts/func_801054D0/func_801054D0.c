#include "common.h"

typedef struct {
    s32 _0;
    s32 _4;
    s32 _8;
    s32 _C;
} Vp16;

extern Vp16 D_8004A928[];
extern s32 D_8004A910;
extern Vp16 D_8004A8B8;

extern void func_80007850(void);

void func_801054D0(void) {
    D_8004A8B8 = D_8004A928[D_8004A910];
    func_80007850();
}
