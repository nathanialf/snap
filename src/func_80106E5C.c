#include "common.h"

extern s32 D_8004A910;
extern s32 D_8004A848[];

extern void func_800054D0(void);
extern void func_800055D0(void);
extern void func_80005F34(void);
extern void func_80007AFC(s32);
extern void func_80005A1C(void);

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
