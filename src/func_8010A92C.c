#include "common.h"

extern void *D_8004AC44;

extern void func_8000A828(void *, s32, s32, s32, s32);
extern void func_80008650(void *, void *);

void func_8010A92C(void *arg0, s32 arg1, s32 arg2, void *arg3) {
    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    func_8000A828(arg0, arg1, *(u8 *) ((u8 *) arg3 + 0xD), *(s32 *) ((u8 *) arg3 + 0x28), arg2);
    func_80008650(arg0, arg3);
}

void func_8010A980(void *arg0, s32 arg1, s32 arg2, void *arg3) {
    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    func_8000A828(arg0, arg1, *(u8 *) ((u8 *) arg3 + 0xD), *(s32 *) ((u8 *) arg3 + 0x28), arg2);
    func_80008650(arg0, *(s32 *) ((u8 *) arg3 + 0x8));
}
