#include "common.h"

extern void func_80034830(s8 *, s32);

void func_80136124(s32 arg0) {
    s8 buf;
    func_80034830(&buf, *(s32 *) &arg0);
}
