#include "common.h"

extern s16 D_80049798;
extern s16 D_8004979A;

typedef struct {
    int x, y;
} Vec2i;

void func_801075FC(Vec2i v) {
    D_80049798 = v.x;
    D_8004979A = v.y;
}
