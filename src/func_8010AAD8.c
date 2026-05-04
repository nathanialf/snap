#include "common.h"

extern void *D_8004AC44;

extern void func_8000A9D8(void *, s32, s32, s32, void *);
extern void func_80008650(void *, void *);

void func_8010AAD8(void *arg0, s32 arg1, s32 arg2, void *arg3, void *arg4) {
    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    func_8000A9D8(arg0, arg1, *(s32 *) ((u8 *) arg4 + 0x28), arg2, arg3);
    func_80008650(arg0, *(s32 *) ((u8 *) arg4 + 0x8));
}
