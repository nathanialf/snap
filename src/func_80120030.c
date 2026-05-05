#include "common.h"

typedef struct {
    s32 a;
    char _4[4];
    u8  type;
    u8  visited;
    char _A[2];
    s32 c;
    s32 d;
} Foo;

void func_80120030(Foo *arg0, s32 arg1, s32 arg2) {
    if (arg0->visited) {
        return;
    }
    arg0->visited = 1;
    arg0->a += arg2;
    if (arg0->type == 0) {
        arg0->d += arg1;
        if (arg0->c != 0) {
            arg0->c += arg1;
        }
    } else if (arg0->type == 1) {
        if (arg0->c != 0) {
            arg0->c += arg1;
        }
    }
}
