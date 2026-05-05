#include "common.h"

extern u8 D_800007C4[];
extern u8 D_80045870[];
extern u8 D_80045B20[];
extern u8 D_800486A8[];
extern u64 D_800456A8;
extern u64 D_80045A58;

extern void func_8003D8A0(s32);
extern void func_80034E30(void);
extern void func_80034A00(u8 *, s32, u8 *, u8 *, u8 *, s32);
extern void func_8003D0F0(u8 *);

void func_80100870(void) {
    D_800456A8 = 0xFEDCBA98ULL;
    func_8003D8A0(0x04900000);
    func_80034E30();
    func_80034A00(D_80045870, 1, D_800007C4, D_800486A8, D_80045B20, 0x7F);
    D_80045A58 = 0xFEDCBA98ULL;
    func_8003D0F0(D_80045870);
}
