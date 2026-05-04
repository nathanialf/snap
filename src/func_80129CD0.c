#include "common.h"

extern void *func_8002AB70(s32 *, void *);
extern s32 func_8002AC9C(s32 *, s32);

typedef struct VtabH {
    char pad[8];
    void (*method)(struct VtabH *self, s32 cmd);
} VtabH;

typedef struct InnerH {
    char pad[0xC];
    VtabH *vtab;
    char pad2[0xD8 - 0x10];
    s32 ofs_d8;
} InnerH;

typedef struct OuterH {
    char pad[8];
    InnerH *inner;
} OuterH;

void func_80129CD0(arg0, arg1, arg2, arg3)
s32 *arg0;
OuterH *arg1;
s16 arg2;
s32 arg3;
{
    s32 *m;
    if (arg1->inner != 0) {
        m = (s32 *) func_8002AB70(arg0, arg1);
        if (m != 0) {
            *(s16 *) ((u8 *) m + 8) = 0xB;
            m[1] = arg0[0x1C / 4] + arg1->inner->ofs_d8;
            m[3] = arg2;
            m[4] = func_8002AC9C(arg0, arg3);
            m[0] = 0;
            arg1->inner->vtab->method(arg1->inner->vtab, 3);
        }
    }
}
