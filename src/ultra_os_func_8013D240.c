#include "common.h"

typedef struct Foo_s {
    u16 type;
    char pad0[0x22];
    f32 _f24;
} Foo;

extern Foo *D_80042EB4;

extern u32 func_80038D70(void);
extern void func_80038D90(u32);

void func_8013D240(f32 arg0) {
    register u32 saveMask = func_80038D70();
    D_80042EB4->_f24 = arg0;
    D_80042EB4->type |= 4;
    func_80038D90(saveMask);
}
