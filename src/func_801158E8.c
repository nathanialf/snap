#include "common.h"

typedef struct {
    char pad[0x3C];
    f32 x;
    f32 y;
    f32 z;
} Pos;

typedef struct {
    char pad[0x48];
    Pos *target;
} CtxOuter;

extern CtxOuter *D_8004AC48;

f32 func_801158E8(s32 *arg0) {
    Pos *p = D_8004AC48->target;
    f32 dx = *(f32 *) ((u8 *) arg0 + 0x1C) - p->x;
    f32 dy = *(f32 *) ((u8 *) arg0 + 0x20) - p->y;
    f32 dz = *(f32 *) ((u8 *) arg0 + 0x24) - p->z;
    return dx * dx + dy * dy + dz * dz;
}
