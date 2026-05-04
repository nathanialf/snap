#include "common.h"

extern void *func_8002AB70(s32 *, void *);
extern void func_8002AC18(s32 *, void *);

typedef struct VtabZ {
    char pad[8];
    void (*method)(struct VtabZ *self, s32 cmd);
} VtabZ;

typedef struct InnerZ {
    char pad[0xC];
    VtabZ *vtab;
    char pad2[0xD8 - 0x10];
    s32 ofs_d8;
} InnerZ;

typedef struct OuterZ {
    char pad[8];
    InnerZ *inner;
} OuterZ;

void func_8012A0C0(s32 *arg0, OuterZ *arg1) {
    s32 *m;
    InnerZ *inner = arg1->inner;
    if (inner != 0) {
        if (inner->ofs_d8 != 0) {
            m = (s32 *) func_8002AB70(arg0, arg1);
            if (m != 0) {
                *(s16 *) ((u8 *) m + 8) = 0;
                m[1] = arg0[0x1C / 4] + arg1->inner->ofs_d8;
                m[3] = (s32) arg1->inner;
                arg1->inner->vtab->method(arg1->inner->vtab, 3);
            }
        } else {
            func_8002AC18(arg0, inner);
        }
        arg1->inner = 0;
    }
}
