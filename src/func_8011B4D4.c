#include "common.h"

extern f32 D_80043D90;
extern u16 D_80040F50[];

s32 func_8011B4D4(f32 arg0) {
    s32 idx = (s32) (arg0 * D_80043D90);
    s32 v = D_80040F50[idx & 0x7FF];

    if (idx & 0x800) {
        return -v;
    }
    return v;
}
