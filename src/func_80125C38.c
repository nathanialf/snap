#include "common.h"

extern u8 D_80097338[];
extern u8 D_80097350[];
extern u8 D_80096988[];

extern void func_80025AA4(void);

extern void func_80037700(u8 *, u8 *, s32);
extern void func_80034A00(u8 *, s32, void *, s32, u8 *, s32);
extern void func_8003D0F0(u8 *);

void func_80125C38(void) {
    func_80037700(D_80097338, D_80097350, 1);
    func_80034A00(D_80096988, 8, func_80025AA4, 0, D_80097338, 0xFA);
    func_8003D0F0(D_80096988);
}
