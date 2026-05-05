#include "common.h"

extern u8 *D_8004AC44;
extern s32 D_8004AC54;

extern void func_8000BE5C(void *);
extern void func_8000B740(void *);
extern void func_8000C1CC(void *);
extern void func_8000C220(void *);
extern void func_8000A2B0(void *);
extern void func_800087AC(void *);
extern void func_800085E8(void *);
extern void func_80008460(void *);

void func_8010A52C(u8 *arg0) {
    if (arg0 == 0 || arg0 == D_8004AC44) {
        D_8004AC54 = 2;
        return;
    }
    func_8000BE5C(arg0);
    func_8000B740((u8 *) arg0 + 0x3C);
    switch (arg0[0xF]) {
        case 1: func_8000C1CC(arg0); break;
        case 2: func_8000C220(arg0); break;
        case 3: func_8000A2B0(*(void **) ((u8 *) arg0 + 0x48)); break;
    }
    if (arg0[0xD] != 0x21) {
        func_800087AC(arg0);
    }
    func_800085E8(arg0);
    func_80008460(arg0);
}
