#include "common.h"

extern s16 D_8004A904;
extern s16 D_8004A906;

typedef struct {
    int x, y;
} Vec2i;

void func_80105454(Vec2i v) {
    D_8004A904 = v.x;
    D_8004A906 = v.y;
}
