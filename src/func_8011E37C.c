#include "common.h"

typedef struct {
    f32 m[16];
} Mtx;

extern void func_8001E2B0(Mtx *, f32, f32);
extern void func_8001B320(Mtx *, s32);

void func_8011E37C(s32 arg0, f32 arg1, f32 arg2) {
    Mtx mat;
    func_8001E2B0(&mat, arg1, arg2);
    func_8001B320(&mat, arg0);
}
