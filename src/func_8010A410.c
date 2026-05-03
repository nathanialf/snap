#include "common.h"

extern s32 func_8000A344(s32, s32, s32);
extern void func_800084F4(s32);
extern void func_80008560(s32);

s32 func_8010A410(arg0, arg1, arg2)
s32 arg0;
s32 arg1;
int arg2;
{
    s32 result;
    s32 *home = &arg2;

    result = func_8000A344(arg0, arg1, arg2 & 0xFF);

    if (result == 0) {
        return 0;
    }
    func_800084F4(result);
    return result;
}

s32 func_8010A450(arg0, arg1, arg2)
s32 arg0;
s32 arg1;
int arg2;
{
    s32 result;
    s32 *home = &arg2;

    result = func_8000A344(arg0, arg1, arg2 & 0xFF);

    if (result == 0) {
        return 0;
    }
    func_80008560(result);
    return result;
}
