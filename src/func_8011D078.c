#include "common.h"

typedef struct {
    f32 m[16];
} Mtx;

extern void func_8001CEF8(Mtx *, f32, f32, f32, f32);
extern void func_8001B320(Mtx *, s32);

void func_8011D078(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    Mtx mat;
    func_8001CEF8(&mat, arg1, arg2, arg3, arg4);
    func_8001B320(&mat, arg0);
}
