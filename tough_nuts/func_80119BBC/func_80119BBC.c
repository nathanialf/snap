#include "common.h"

extern f32 D_80043D68;
extern f32 D_80043D6C;
extern f32 D_80043D70;
extern f32 D_80043D74;

extern f32 func_80036090(f32);
extern f32 func_80019994(f32);

f32 func_80119BBC(f32 arg0) {
    if (D_80043D68 < arg0) {
        return D_80043D6C;
    }
    if (arg0 < D_80043D70) {
        return D_80043D74;
    }
    {
        f32 t = arg0 * arg0;
        return func_80019994(arg0 / func_80036090(1.0f - t));
    }
}
