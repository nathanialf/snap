#include "common.h"

extern f32 D_80043D68;
extern f32 D_80043D6C;
extern f32 D_80043D70;
extern f32 D_80043D74;
extern f32 func_80036090(f32);
extern f32 func_80019994(f32);

f32 func_80119BBC(f32 x) {
    if (x > D_80043D68) {
        return D_80043D6C;
    }
    if (x < D_80043D70) {
        return D_80043D74;
    }
    return func_80019994(x / func_80036090(1.0f - x * x));
}
