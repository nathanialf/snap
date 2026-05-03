#include "common.h"

extern s32 func_80009D70(void);
extern s32 func_80009E20(void);
extern s32 func_80009E94(void);
extern void func_8000FAFC(s32);

s32 func_8010FB34(void) {
    s32 v = func_80009D70();
    func_8000FAFC(v);
    return v;
}

s32 func_8010FB64(void) {
    s32 v = func_80009E20();
    func_8000FAFC(v);
    return v;
}

s32 func_8010FB94(void) {
    s32 v = func_80009E94();
    func_8000FAFC(v);
    return v;
}
