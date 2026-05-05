#include "common.h"

typedef struct Foo {
    s32 _0;
    s32 _4;
    s32 _8;
} Foo;

extern Foo *D_8004873C;

extern void func_80035720(void);
extern void func_80000D54(Foo *);
extern void func_80032CFC(void *);
extern void func_80032E8C(void *);

void func_80101810(Foo *arg0) {
    if (D_8004873C != 0) {
        func_80035720();
        D_8004873C->_8 = 4;
        func_80000D54(D_8004873C);
        arg0->_8 = 3;
    } else {
        func_80032CFC((u8 *) arg0 + 0x28);
        func_80032E8C((u8 *) arg0 + 0x28);
        arg0->_8 = 2;
    }
    D_8004873C = arg0;
}
