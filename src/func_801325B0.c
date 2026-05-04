#include "common.h"

extern void *func_8002AB70(s32 *, void *);

typedef struct Vtab {
    char pad[8];
    void (*method)(struct Vtab *self, s32 cmd);
} Vtab;

typedef struct Inner {
    char pad[0xC];
    Vtab *vtab;
    char pad2[0xD8 - 0x10];
    s32 ofs_d8;
} Inner;

typedef struct Outer {
    char pad[8];
    Inner *inner;
} Outer;

void func_801325B0(s32 *arg0, Outer *arg1) {
    s32 *m;
    if (arg1->inner != 0) {
        m = (s32 *) func_8002AB70(arg0, arg1);
        if (m != 0) {
            *(s16 *) ((u8 *) m + 8) = 0xF;
            m[0] = 0;
            m[1] = arg0[0x1C / 4] + arg1->inner->ofs_d8;
            arg1->inner->vtab->method(arg1->inner->vtab, 3);
        }
    }
}
