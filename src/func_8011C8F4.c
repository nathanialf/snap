#include "common.h"

typedef struct {
    f32 m[16];
} Mtx;

extern void func_8001C6C8(Mtx *, s32, f32, f32, f32, f32, f32);
extern void func_8001B0F0(Mtx *, s32);

void func_8011C8F4(s32 arg0, s32 arg1, f32 arg2, f32 arg3,
                   f32 arg4, f32 arg5, f32 arg6) {
    Mtx mat;
    func_8001C6C8(&mat, arg1, arg2, arg3, arg4, arg5, arg6);
    func_8001B0F0(&mat, arg0);
}
