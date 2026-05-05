#include "common.h"

typedef struct Bar {
    u8 _0[0x48];
    struct Bar *_48;
} Bar;

extern Bar *D_8004AC48;
extern u8 D_8004A890[];

extern void func_80005F34(void);
extern void func_80005A98(void);
extern void func_80017ADC(u8 *, Bar *, s32);
extern void func_8001837C(u8 *, Bar *);
extern void func_80018CDC(Bar *, s32);

void func_80119278(void) {
    Bar *p;

    p = D_8004AC48->_48;
    func_80005F34();
    func_80005A98();
    func_80017ADC(D_8004A890, p, 0);
    func_8001837C(D_8004A890, p);
    func_80018CDC(p, 0);
}
