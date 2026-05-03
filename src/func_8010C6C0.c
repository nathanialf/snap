#include "common.h"

extern void func_80009984(void *);
extern void *func_8000C550(void *);

void func_8010C6C0(s32 *arg0) {
    void *p = (void *) arg0[0x12];
    while (p != 0) {
        func_80009984(p);
        p = func_8000C550(p);
    }
}
