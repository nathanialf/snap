#include "common.h"

extern void func_80008960(void *);
extern f32 D_800436EC;

void func_80109A50(s32 *arg0) {
    void *p = (void *) arg0[0x24];
    while (p != 0) {
        void *next = *(void **) p;
        func_80008960(p);
        p = next;
    }
    arg0[0x24] = 0;
    *(f32 *) ((u8 *) arg0 + 0x98) = D_800436EC;
}
