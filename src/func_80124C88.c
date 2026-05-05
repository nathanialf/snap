#include "common.h"

extern s32 func_8000BC58(s32);
extern s32 func_8000C3FC(s32, void *, s32, s32, void *, s32, s32, s32, s32, s32, s32, s32, s32);
extern u8 D_80024450[];
extern u8 D_8000BC84[];

s32 func_80124C88(arg0, arg1, arg2)
s32 arg0;
s32 arg1;
s32 arg2;
{
    if (func_8000BC58(-2) != 0) {
        return 0;
    }
    return func_8000C3FC(-2, D_8000BC84, arg0, arg1, D_80024450, arg2, 0, 0, 0, 0, 0, 0, 0);
}
