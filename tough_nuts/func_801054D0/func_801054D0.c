#include "common.h"

typedef struct Slot {
    u32 _0;
    u32 _4;
    u32 _8;
    u32 _C;
} Slot;

extern void func_80007850(Slot *);
extern s32 D_8004A910;
extern Slot D_8004A928[];
extern Slot D_8004A8B8;

void func_801054D0(void) {
    D_8004A8B8 = D_8004A928[D_8004A910];
    func_80007850(&D_8004A8B8);
}
