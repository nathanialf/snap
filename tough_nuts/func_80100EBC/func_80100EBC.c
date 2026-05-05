#include "common.h"

typedef struct {
    s32 data[20];
} Block50;

extern Block50 D_80048758;
extern Block50 D_800487A8;
extern u32 D_8004882C;
extern u32 D_800487F8;

extern void func_80035740(void);
extern void func_80035660(s32);

void func_80100EBC(void) {
    D_80048758 = D_800487A8;
    func_80035740();
    func_80035660(((u32) D_8004882C << 4) >> 31);
    D_800487F8 = 0;
}
