#include "common.h"

extern void func_80025F24(const char *);
extern void func_8000577C(s32, s32);

extern s32 D_80040CF0;
extern s32 D_8004A8F8;
extern s32 D_8004A8FC;
extern char D_800430C8[];

void func_801057D4(s32 arg0, s32 arg1, s32 arg2) {
    D_80040CF0 = arg0;
    D_8004A8F8 = arg1;
    D_8004A8FC = arg2;
    if ((arg0 == 2 || arg0 == 1) && arg2 == 0) {
        func_80025F24(D_800430C8);
        for (;;) {
        }
    }
    if (arg0 == 1) {
        func_8000577C(arg1, arg2);
    }
}
