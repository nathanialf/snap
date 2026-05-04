#include "common.h"

extern void *func_8002AB70(s32 *, void *);

typedef struct VtabF {
    char pad[8];
    void (*method)(struct VtabF *self, s32 cmd);
} VtabF;

typedef struct InnerF {
    char pad[0xC];
    VtabF *vtab;
    char pad2[0xD8 - 0x10];
    s32 ofs_d8;
} InnerF;

typedef struct OuterF {
    char pad[8];
    InnerF *inner;
} OuterF;

void func_8012A520(s32 *arg0, OuterF *arg1, f32 arg2) {
    s32 *m;
    if (arg1->inner != 0) {
        m = (s32 *) func_8002AB70(arg0, arg1);
        if (m != 0) {
            *(s16 *) ((u8 *) m + 8) = 7;
            m[1] = arg0[0x1C / 4] + arg1->inner->ofs_d8;
            m[0] = 0;
            *(f32 *) ((u8 *) m + 0xC) = arg2;
            arg1->inner->vtab->method(arg1->inner->vtab, 3);
        }
    }
}
