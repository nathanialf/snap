#include "common.h"

extern u32 func_80038D70(void);
extern void func_80038D90(u32);
extern void func_800338BC(void *);

extern void *D_80042CC0;
extern s32 D_80042CB8;

void func_8013B4E0(void) {
    register u32 saveMask = func_80038D70();
    *(s16 *) ((u8 *) D_80042CC0 + 0x10) = 2;
    func_800338BC(&D_80042CB8);
    func_80038D90(saveMask);
}
