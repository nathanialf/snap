#include "common.h"

extern u32 func_80038D70(void);
extern void func_80038D90(u32);

typedef struct Foo_s {
    u16 type;
    s16 _h2;
    s32 _w4;
    struct Foo_s *cur;
    s32 _wc;
    s32 _w10;
    s32 _w14;
} Foo;

extern Foo *D_80042EB4;

void func_80137B10(s32 arg0) {
    u32 saveMask = func_80038D70();
    D_80042EB4->_w4 = arg0;
    D_80042EB4->type |= 0x10;
    func_80038D90(saveMask);
}
