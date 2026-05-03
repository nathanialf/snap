#include "common.h"

extern void func_800087AC(void *, s32);
extern void func_800086B8(void *);
extern void func_80008724(void *);

void func_8010ACAC(void *arg0, s32 arg1) {
    func_800087AC(arg0, arg1);
    *(s32 *) ((u8 *) arg0 + 0x28) = arg1;
    func_800086B8(arg0);
}

void func_8010ACE0(void *arg0, s32 arg1) {
    func_800087AC(arg0, arg1);
    *(s32 *) ((u8 *) arg0 + 0x28) = arg1;
    func_80008724(arg0);
}
