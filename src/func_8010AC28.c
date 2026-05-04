#include "common.h"

extern void func_800087AC(void *, void *);
extern void func_80008650();

void func_8010AC28(void *arg0, void *arg1) {
    func_800087AC(arg0, arg1);
    *(u8 *) ((u8 *) arg0 + 0xD) = *(u8 *) ((u8 *) arg1 + 0xD);
    *(s32 *) ((u8 *) arg0 + 0x28) = *(s32 *) ((u8 *) arg1 + 0x28);
    func_80008650(arg0);
}

void func_8010AC68(void *arg0, void *arg1) {
    func_800087AC(arg0, arg1);
    *(u8 *) ((u8 *) arg0 + 0xD) = *(u8 *) ((u8 *) arg1 + 0xD);
    *(s32 *) ((u8 *) arg0 + 0x28) = *(s32 *) ((u8 *) arg1 + 0x28);
    func_80008650(arg0, *(s32 *) ((u8 *) arg1 + 0x24));
}
