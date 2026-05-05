#include "common.h"

extern u8 D_8004A8D8[];
extern void func_80006E24(void);
extern void func_80006E5C(void);

extern void func_80005470(void *, s32);
extern void func_800070B8(s32 *, s32);

void func_80107354(arg0)
s32 *arg0;
{
    func_80005470((void *) arg0[3], arg0[4]);
    *(void **) (D_8004A8D8 + 8) = func_80006E24;
    *(void **) (D_8004A8D8 + 0x10) = func_80006E5C;
    func_800070B8(arg0, 0);
}
