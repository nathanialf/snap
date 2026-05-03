#include "common.h"

extern s32 D_8004A958;
extern s32 D_8004A95C;
extern s32 D_8004A960;
extern s32 D_8004A964;
extern s16 D_8004A974;
extern s16 D_8004A976;
extern s16 D_8004A978;
extern s16 D_8004A97A;

void func_80107A94(u8 *arg0) {
    *(s32 *) (arg0 + 0x24) = D_8004A958;
    *(s32 *) (arg0 + 0x28) = D_8004A95C;
    *(s32 *) (arg0 + 0x2C) = D_8004A960;
    *(s16 *) (arg0 + 0x30) = D_8004A974;
    *(s16 *) (arg0 + 0x32) = D_8004A976;
    *(s16 *) (arg0 + 0x34) = D_8004A978;
    *(s16 *) (arg0 + 0x36) = D_8004A97A;
    D_8004A960 = 0;
    D_8004A964 = 0;
}
