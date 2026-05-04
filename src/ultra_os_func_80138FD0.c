#include "common.h"

extern u32 func_80038D70(void);
extern void func_80038D90(u32);

typedef struct Foo_s {
    s16 type;
    s16 _h2;
    s32 _w4;
    struct Foo_s *cur;
    s32 _wc;
    s32 _w10;
    s32 _w14;
} Foo;

extern Foo *D_80042EB4;

void func_80138FD0(s32 arg0, s32 arg1, s16 arg2) {
    register u32 saveMask = func_80038D70();
    D_80042EB4->_w10 = arg0;
    D_80042EB4->_w14 = arg1;
    D_80042EB4->_h2 = arg2;
    func_80038D90(saveMask);
}
