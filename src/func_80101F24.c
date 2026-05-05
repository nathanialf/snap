#include "common.h"

typedef struct Item {
    s32 _0;
    s32 _4;
    s32 _8;
    u8 _C[0x44];
    s32 _50;
    u8 _54[0x24];
    s32 _78;
} Item;

extern Item *D_80048754;
extern Item *D_8004874C;

extern void func_80000E6C(Item *);
extern void func_8003CCA0(s32, s64);

void func_80101F24(void) {
    Item *p;

    if (D_80048754 != 0) {
        return;
    }
    p = D_8004874C;
    if (p == 0) {
        return;
    }
    D_80048754 = p;
    func_80000E6C(p);
    D_80048754->_8 = 2;
    func_8003CCA0(D_80048754->_50, (s64)D_80048754->_78);
}
