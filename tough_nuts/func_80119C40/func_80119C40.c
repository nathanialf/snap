#include "common.h"

extern f32 D_80043D78;
extern f32 D_80043D7C;
extern f32 D_80043D80;
extern f32 D_80043D84;

extern f32 func_80036090(f32);
extern f32 func_80019994(f32);

f32 func_80119C40(f32 arg0) {
    if (D_80043D78 < arg0) {
        return 0.0f;
    }
    if (arg0 < D_80043D7C) {
        return D_80043D80;
    }
    return D_80043D84 - func_80019994(arg0 / func_80036090(1.0f - arg0 * arg0));
}
