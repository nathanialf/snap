#include "common.h"

typedef struct {
    f32 m[16];
} Mtx;

extern void func_8001E460(Mtx *, f32);
extern void func_8001B320(Mtx *, s32);

void func_8011E4F0(s32 arg0, f32 arg1) {
    Mtx mat;
    func_8001E460(&mat, arg1);
    func_8001B320(&mat, arg0);
}
