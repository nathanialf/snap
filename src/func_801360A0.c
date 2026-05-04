#include "common.h"

extern u8 D_80097EF0;
extern s32 D_80097EB0;

extern void func_80034650(s32);
extern void func_80034900(s32);
extern s32 func_80035560(s32, s32 *);
extern void func_80032840(s32, s32, s32);
extern void func_80034694(void);

s32 func_801360A0(s32 arg0) {
    s32 result = 0;
    func_80034650(arg0);
    if (D_80097EF0 != 0) {
        func_80034900(0);
        result = func_80035560(1, &D_80097EB0);
        func_80032840(arg0, 0, 1);
    }
    result = func_80035560(0, &D_80097EB0);
    D_80097EF0 = 0;
    func_80034694();
    return result;
}
