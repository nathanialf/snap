#include "common.h"

extern void *func_8002AB70(s32 *, void *);

typedef struct VtabB {
    char pad[8];
    void (*method)(struct VtabB *self, s32 cmd);
} VtabB;

typedef struct InnerB {
    char pad[0xC];
    VtabB *vtab;
    char pad2[0xD8 - 0x10];
    s32 ofs_d8;
} InnerB;

typedef struct OuterB {
    char pad[8];
    InnerB *inner;
} OuterB;

void func_8012A5B0(arg0, arg1, arg2)
s32 *arg0;
OuterB *arg1;
unsigned char arg2;
{
    s32 *m;
    if (arg1->inner != 0) {
        m = (s32 *) func_8002AB70(arg0, arg1);
        if (m != 0) {
            *(s16 *) ((u8 *) m + 8) = 0xC;
            m[1] = arg0[0x1C / 4] + arg1->inner->ofs_d8;
            m[0] = 0;
            m[3] = arg2;
            arg1->inner->vtab->method(arg1->inner->vtab, 3);
        }
    }
}
