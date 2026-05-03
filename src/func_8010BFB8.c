#include "common.h"

extern s32 func_80009D70(void);
extern s32 func_80009E20(void);
extern s32 func_80009E94(void);
extern void func_8000BEAC(s32);
extern void func_8000BEF4(s32);

s32 func_8010BFB8(void) {
    s32 v = func_80009D70();
    func_8000BEAC(v);
    return v;
}

s32 func_8010BFE8(void) {
    s32 v = func_80009E20();
    func_8000BEAC(v);
    return v;
}

s32 func_8010C018(void) {
    s32 v = func_80009E94();
    func_8000BEAC(v);
    return v;
}

s32 func_8010C048(void) {
    s32 v = func_80009D70();
    func_8000BEF4(v);
    return v;
}

s32 func_8010C078(void) {
    s32 v = func_80009E20();
    func_8000BEF4(v);
    return v;
}

s32 func_8010C0A8(void) {
    s32 v = func_80009E94();
    func_8000BEF4(v);
    return v;
}
