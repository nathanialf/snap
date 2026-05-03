#include "common.h"

extern void func_80036310(s32 *);
extern void func_80036210(s32 *, s32);

void func_80136398(s32 arg0) {
    s32 buf[16];
    func_80036310(buf);
    func_80036210(buf, arg0);
}
