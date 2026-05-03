#include "common.h"

extern s16 D_8004A974;
extern s16 D_8004A976;
extern s16 D_8004A978;
extern s16 D_8004A97A;
extern s32 D_8004A964;

void func_80107A34(arg0, arg1, arg2, arg3)
s16 arg0;
s16 arg1;
s16 arg2;
s16 arg3;
{
    D_8004A974 = arg0;
    D_8004A976 = arg1;
    D_8004A978 = arg2;
    D_8004A97A = arg3;
    D_8004A964 = 1;
}
