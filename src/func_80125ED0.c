#include "common.h"

extern u8 D_80097388[];
extern u8 D_80097D38[];
extern void D_80025CA4(void);

extern void func_80034A00(u8 *, s32, void (*)(void), s32, u8 *, s32);
extern void func_8003D0F0(u8 *);

void func_80125ED0(void) {
    func_80034A00(D_80097388, 8, D_80025CA4, 0, D_80097D38, 0x69);
    func_8003D0F0(D_80097388);
}
