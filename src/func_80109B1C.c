#include "common.h"

extern void func_80008960(void *);
extern f32 D_800436F0;

void func_80109B1C(s32 *arg0) {
    void *p = (void *) arg0[0x1B];
    while (p != 0) {
        void *next = *(void **) p;
        func_80008960(p);
        p = next;
    }
    arg0[0x1B] = 0;
    *(f32 *) ((u8 *) arg0 + 0x74) = D_800436F0;
}
