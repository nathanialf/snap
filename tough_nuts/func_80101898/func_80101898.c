#include "common.h"

typedef struct Foo {
    s32 _0;
    s32 _4;
    s32 _8;
} Foo;

extern Foo *D_8004873C;
extern Foo *D_80048740;
extern s32 D_80048820;

extern Foo *func_80036010(Foo *);
extern void func_80035720(void);
extern void func_80032CFC(void *);
extern void func_80032E8C(void *);

void func_80101898(Foo *arg0) {
    Foo *new_var2;
    int new_var;
    new_var = 2;
    D_80048820 = (s32) func_80036010(arg0);
    if ((D_8004873C != 0) && (D_8004873C->_8 == new_var)) {
        func_80035720();
        D_8004873C->_8 = 4;
        new_var2->_8 = 3;
    } else {
        func_80032CFC(((u8 *) new_var2) + 0x28);
        func_80032E8C(((u8 *) new_var2) + 0x28);
        if (1) {
            new_var2->_8 = 2;
            new_var2 = arg0;
        }
    }
    D_80048740 = arg0;
}
