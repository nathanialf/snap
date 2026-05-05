#include "common.h"

typedef struct Foo {
    u8 _0[0x70];
    s32 idx;
} Foo;

extern Foo *D_80048800[3];
extern Foo *D_8004880C;
extern Foo *D_80048810;
extern Foo *D_8004881C;
extern s32 D_80048838;

extern Foo *func_80035270(void);
extern Foo *func_8003D820(void);
extern Foo *func_80036010(void);

s32 func_80100A64(Foo *arg0) {
    Foo *a;
    Foo *b;
    Foo *p;
    s32 i;

    if (D_8004880C != 0) {
        return 1;
    }
    if (D_80048810 != 0) {
        return 0;
    }
    a = func_80035270();
    b = func_8003D820();

    if (arg0->idx != -1) {
        p = D_80048800[arg0->idx];
        if (p != 0 && b != p && a != p) {
            D_8004880C = p;
            D_80048810 = p;
            D_80048838 = 0;
            D_8004881C = func_80036010();
            return 1;
        }
    }

    for (i = 0; i < 3; i++) {
        p = D_80048800[i];
        if (p != 0 && b != p && a != p) {
            D_8004880C = p;
            D_80048838 = 0;
            D_8004881C = func_80036010();
            return 1;
        }
    }

    return 0;
}
