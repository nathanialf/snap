#include "common.h"

extern u32 func_80038D70(void);
extern void func_80038D90(u32);
extern u16 *D_80042EB4;

void func_80135660(arg0)
unsigned char arg0;
{
    register u32 saveMask = func_80038D70();
    if (arg0 != 0) {
        *D_80042EB4 |= 0x20;
    } else {
        *D_80042EB4 &= ~0x20;
    }
    func_80038D90(saveMask);
}
