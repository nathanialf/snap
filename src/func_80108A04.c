#include "common.h"

extern void *D_8004AC18;
extern s32 D_8004AC1C;
extern void *D_8004AC20;
extern s32 D_8004AC24;
extern u16 D_8004AC28;
extern u8 D_80043364;
extern void *D_8004AC2C;
extern s32 D_8004AC30;
extern u16 D_8004AC34;
extern u8 D_8004337C;
extern void *D_8004AC38;
extern s32 D_8004AC3C;
extern u16 D_8004AC40;
extern u8 D_80043394;

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *);

void func_80108A04(void **arg0) {
    arg0[0] = D_8004AC18;
    D_8004AC18 = arg0;
    D_8004AC1C--;
}

void *func_80108A30(void) {
    void *r;
    if (D_8004AC20 == 0) {
        D_8004AC20 = func_800054A0(D_8004AC28, 8);
    }
    if (D_8004AC20 == 0) {
        func_80025F24(&D_80043364);
        while (1) {}
    }
    r = D_8004AC20;
    D_8004AC20 = *(void **) D_8004AC20;
    D_8004AC24++;
    return r;
}

void func_80108AAC(void **arg0) {
    arg0[0] = D_8004AC20;
    D_8004AC20 = arg0;
    D_8004AC24--;
}

void *func_80108AD8(void) {
    void *r;
    if (D_8004AC2C == 0) {
        D_8004AC2C = func_800054A0(D_8004AC34, 8);
    }
    if (D_8004AC2C == 0) {
        func_80025F24(&D_8004337C);
        while (1) {}
    }
    r = D_8004AC2C;
    D_8004AC2C = *(void **) D_8004AC2C;
    D_8004AC30++;
    return r;
}

void func_80108B54(void **arg0) {
    arg0[0] = D_8004AC2C;
    D_8004AC2C = arg0;
    D_8004AC30--;
}

void *func_80108B80(void) {
    void *r;
    if (D_8004AC38 == 0) {
        D_8004AC38 = func_800054A0(D_8004AC40, 8);
    }
    if (D_8004AC38 == 0) {
        func_80025F24(&D_80043394);
        while (1) {}
    }
    r = D_8004AC38;
    D_8004AC38 = *(void **) D_8004AC38;
    D_8004AC3C++;
    return r;
}

void func_80108BFC(void **arg0) {
    arg0[0] = D_8004AC38;
    D_8004AC38 = arg0;
    D_8004AC3C--;
}
