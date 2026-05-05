#include "common.h"

extern void func_8001A360(f32 *);

void func_8011A5F4(f32 *v, f32 scale, f32 *offset_src, f32 offset_scale) {
    v[0] *= scale;
    v[1] *= scale;
    v[2] *= scale;
    v[0] += offset_src[0] * offset_scale;
    v[1] += offset_src[1] * offset_scale;
    v[2] += offset_src[2] * offset_scale;
    func_8001A360(v);
}
