#include "common.h"

extern void func_80009760(s32, s32, s32);
extern void func_80009790(s32, s32, s32);
extern void func_80009C94(void *);
extern void *func_8000C550(void *);
extern s32 func_80009D70(void);
extern s32 func_80009E20(void);
extern s32 func_80009E94(void);
extern void func_8000BEAC(s32);
extern void func_8000BEF4(s32);

void func_8010BEAC(s32 arg0) {
    func_80009760(arg0, 0x12, 0);
    func_80009760(arg0, 0x15, 0);
    func_80009760(arg0, 0x20, 0);
}

void func_8010BEF4(s32 arg0) {
    func_80009760(arg0, 0x12, 0);
    func_80009760(arg0, 0x1A, 0);
    func_80009760(arg0, 0x20, 0);
}

void func_8010BF3C(s32 arg0) {
    func_80009790(arg0, 3, 0);
    func_80009790(arg0, 6, 0);
}

void func_8010BF74(s32 *arg0) {
    void *p = (void *) arg0[0x12];
    while (p != 0) {
        func_80009C94(p);
        p = func_8000C550(p);
    }
}

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
