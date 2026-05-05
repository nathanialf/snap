#include "common.h"

typedef char *va_list;
#define _VA_ALIGN(p, a) (((unsigned int)(((char *)p) + ((a) > 4 ? (a) : 4) - 1)) & -((a) > 4 ? (a) : 4))
#define va_start(vp, parmN) (vp = ((va_list)&parmN + sizeof(parmN)))
#define va_end(list)

extern void func_80025938(s32 arg0, va_list ap);

void func_80125A68(s32 arg0, ...) {
    va_list ap;
    char *base;

    base = (char *)&arg0;
    ap = base + sizeof(arg0);
    func_80025938(arg0, (va_list)_VA_ALIGN(ap, 4));
}
