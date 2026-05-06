#include "common.h"

extern void (*D_8004A948)();
extern s32 D_8004A910;
extern s32 D_8004A848[];

extern void func_800054D0(void);
extern void func_800055D0(void);
extern void func_80005F34(void);
extern void func_80005A1C(void);
extern void func_80007AFC(s32);
extern s32 func_80006804(void);
extern void func_8000C274(void);

void func_80106E24(s32 arg0) {
    D_8004A948(arg0);
    (*(void (**)()) ((u8 *) arg0 + 0x4))();
}

void func_80106E5C(arg0)
s32 *arg0;
{
    func_800054D0();
    func_800055D0();
    ((void (*)(void)) arg0[3])();
    func_80005F34();
    func_80007AFC(D_8004A848[D_8004A910]);
    func_80005A1C();
}

void func_80106EC0(s32 *arg0) {
    D_8004A948(arg0);
    ((void (*)(void)) arg0[1])();
    if (func_80006804() != 0) {
        func_8000C274();
    }
}

void func_80106F10(arg0)
s32 *arg0;
{
    func_800054D0();
    func_800055D0();
    ((void (*)(void)) arg0[3])();
    func_80005F34();
    func_80007AFC(D_8004A848[D_8004A910]);
    func_80005A1C();
    if (func_80006804() != 0) {
        func_8000C274();
    }
}
