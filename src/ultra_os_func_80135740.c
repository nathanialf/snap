#include "common.h"

extern u32 func_80038D70(void);
extern void func_80038D90(u32);

typedef struct Foo_s {
    s16 type;
    s16 _pad;
    s32 _w4;
    struct Foo_s *cur;
    s32 next;
} Foo;

extern Foo *D_80042EB4;

void func_80135740(Foo *arg0) {
    register u32 saveMask = func_80038D70();
    D_80042EB4->cur = arg0;
    D_80042EB4->type = 1;
    D_80042EB4->next = D_80042EB4->cur->_w4;
    func_80038D90(saveMask);
}
